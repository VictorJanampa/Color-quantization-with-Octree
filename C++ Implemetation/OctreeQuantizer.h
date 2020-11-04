class OctreeNode;
class OctreeQuantizer
{
public:
	OctreeNode *root;
	std::vector<OctreeNode*> levels[8];
	OctreeQuantizer();
	std::vector<OctreeNode*> get_leaves();
	void add_level_node(int ,OctreeNode *);
	void add_color(Color);
	int get_color_index_for_level(Color,int);
	std::vector<Color> make_palette(int color_count);
	int get_palette_index(Color);
};

