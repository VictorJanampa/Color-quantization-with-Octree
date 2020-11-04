#include <vector>
#include "color.h"
#include "OctreeQuantizer.h"
#include <iostream>

using namespace std;
int MAX_DEPTH = 8;

class OctreeNode
{
public:
	Color color;
	int pixel_count;
	int palette_index;
	std::vector <OctreeNode*> children; 

	OctreeNode(int,OctreeQuantizer*);
	bool is_leaf();
	std::vector<OctreeNode*> get_leaf_nodes();
	int get_nodes_pixel_count();
	void add_color(Color,int,OctreeQuantizer*);
	int get_palette_index(Color,int);
	int remove_leaves();
	int get_color_index_for_level(Color,int);
	Color get_color();
};

Color::Color(int red=0,int green=0,int blue=0,int alpha=0)
{
	this->red=red;
	this->green=green;
	this->blue=blue;
	this->alpha=alpha;
}


OctreeNode::OctreeNode(int level,OctreeQuantizer *parent)
{	
	this->color=Color(0,0,0);
	this->pixel_count = 0;
	this->palette_index = 0;
	this->children.reserve(8);
	for(int i=0;i<MAX_DEPTH;i++)
	{
		children[i]=nullptr;
	}
	if(level<MAX_DEPTH-1)
	{
		parent->add_level_node(level,this);
	}
}

bool OctreeNode::is_leaf()
{
	return this->pixel_count > 0;
}

vector<OctreeNode*> OctreeNode::get_leaf_nodes()
{
	 vector<OctreeNode*> leaf_nodes;
	 for(int i=0;i<8;i++)
	 {
	 	OctreeNode *node = this->children[i];
	 	if(node!=nullptr){
	 		if(node->is_leaf())
	 			leaf_nodes.push_back(node);
	 		else
	 		{
	 			vector<OctreeNode*> aux=node->get_leaf_nodes();
	 			leaf_nodes.insert(leaf_nodes.end(), aux.begin(), aux.end());
	 		}
	 	}
	 }
	 return leaf_nodes;
}
int OctreeNode::get_nodes_pixel_count()
{
	int sum_count=this->pixel_count;
	for(int i=0;i<8;i++)
	{	
		OctreeNode *node = this->children[i];
		if(node != nullptr)
			sum_count+=node->pixel_count;
	}
	return sum_count;
}

void OctreeNode::add_color(Color color,int level,OctreeQuantizer *parent)
{	
	if(level>=MAX_DEPTH)
	{
		this->color.red+=color.red;
		this->color.green+=color.green;
		this->color.blue+=color.blue;
		this->pixel_count+=1;
		return;
	}
	int index=this->get_color_index_for_level(color,level);
	if(this->children[index] == nullptr)
	{
		this->children[index] = new OctreeNode(level,parent);
	}
	this->children[index]->add_color(color,level+1,parent);
}

int OctreeNode::get_palette_index(Color color,int level)
{
	if(this->is_leaf())
	{
		return this->palette_index;
	}
	int index=this->get_color_index_for_level(color,level);
	if(this->children[index])
	{
		return this->children[index]->get_palette_index(color,level+1);
	}
	else
	{
		for(int i=0;i<8;i++)
		{	
			if(this->children[i] != nullptr){
				return this->children[i]->get_palette_index(color,level+1);
			}
		}
	}
	return 0;
}

int OctreeNode::remove_leaves()
{
	int result=0;
	for(int i=0;i<8;i++)
	{	
		OctreeNode *node= this->children[i];
		if(node != nullptr)
		{
			this->color.red+=node->color.red;
			this->color.green+=node->color.green;
			this->color.blue+=node->color.blue;
			this->pixel_count += node->pixel_count;
			result +=1;
		}
	
	}
	return result-1;
}

int OctreeNode::get_color_index_for_level(Color color,int level)
{
	int index=0;
	int mask = 0x80 >> level;
	if (color.red & mask)
		index |= 4;
	if (color.green & mask)
		index |= 2; 
	if (color.blue & mask)
		index |= 1;
	return index;
}

Color OctreeNode::get_color()
{	
	int r=this->color.red/this->pixel_count;
	int g=this->color.green/this->pixel_count;
	int b=this->color.blue/this->pixel_count;
	return Color(r,g,b);
}

OctreeQuantizer::OctreeQuantizer()
{		
		for(int i=0;i<0;i++)
		{
			levels[i].reserve(8);
			for(int j=0;j<8;j++)
			{
				levels[i][j]=nullptr;
			}
		}
		this->root=new OctreeNode(0,this);
}

vector<OctreeNode*> OctreeQuantizer::get_leaves()
	{	
		return this->root->get_leaf_nodes();
	}

void OctreeQuantizer::add_level_node(int level,OctreeNode *node)
	{	
	//	cout<<"add_level_node"<<endl;
		this->levels[level].push_back(node);
	}

void OctreeQuantizer::add_color(Color color)
{
	this->root->add_color(color,0,this);
}
vector<Color> OctreeQuantizer::make_palette(int color_count)
{
	vector<Color>palette;
	int palette_index = 0;
	int leaf_count = this->get_leaves().size();

	for(int level=MAX_DEPTH-1;level>-1;level--)
	{	
		if(this->levels[level].size()>0) 
		{
			for(auto node:this->levels[level])
			{
				leaf_count -=node->remove_leaves();
				if(leaf_count<=color_count)
					break;
			}
			if(leaf_count<=color_count)
				break;
			this->levels[level].clear();
		}
	}
	for (auto node : this->get_leaves()) {
        if (palette_index >= color_count)
            break;
        if (node->is_leaf())
            palette.push_back(node->get_color());
        node->palette_index = palette_index;
        palette_index += 1;
    }
	return palette;
}

int OctreeQuantizer::get_palette_index(Color color)
{
	return this->root->get_palette_index(color,0);
}