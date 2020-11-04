#include "CImg.h"
#include "octreeColor.h"

using namespace cimg_library;

int main()
{
	CImg<float> image("shiva.ppm");
	int width=image.width();
	int height=image.height();
	OctreeQuantizer Oc;
	image.display();
	cimg_forXY(image,x,y) 
	{
		Oc.add_color(Color( (int)image(x,y,0,0) , (int)image(x,y,0,1) , (int)image(x,y,0,2) ) );
	}

	std::vector<Color> palette = Oc.make_palette(256);
	CImg<float> palette_pixel(16,16,1,3);
	palette_pixel.fill(0);
	for (int i=0;i<palette.size();i++)
	{	
		int x=i%16;
		int y=i/16;
		palette_pixel(x,y,0,0) = palette[i].red;
		palette_pixel(x,y,0,1) = palette[i].green;
		palette_pixel(x,y,0,2) = palette[i].blue;
	}
	palette_pixel.display();
	CImg<float> out_image(image);
	cimg_forXY(out_image,x,y)
	{
		int index = Oc.get_palette_index(Color( (int)image(x,y,0,0) ,
												(int)image(x,y,0,1) , 
												(int)image(x,y,0,2) ) );
		Color color = palette[index];
		out_image(x,y,0,0) = color.red;
		out_image(x,y,0,1) = color.green;
		out_image(x,y,0,2) = color.blue;
	}
	out_image.display();
}