#include "TextureData.h"

using namespace CoreLib::Basic;
using namespace CoreLib::Imaging;

namespace CoreLib
{
	namespace Imaging
	{
		int CeilLog2(int val)
		{
			int rs = 0;
			while (val)
			{
				val>>=1;
				rs++;
			}
			return rs;
		}

		TextureData::TextureData()
		{
			Levels.SetSize(2);
			Levels[0].Width = 2;
			Levels[0].Height = 2;
			Levels[0].Pixels.SetSize(4);
			Levels[0].Pixels[2] = Levels[0].Pixels[0] = Color(255, 0, 255, 255);
			Levels[0].Pixels[1] = Levels[0].Pixels[3] = Color(0, 255, 0, 255);
			Levels[1].Width = 1;
			Levels[1].Height = 1;
			Levels[1].Pixels.SetSize(1);
			Levels[1].Pixels[0] = Color(127, 127, 127, 255);
			Width = Height = 2;
			IsTransparent = false;
		}

		TextureData::TextureData(const String & fileName)
		{
			// Read file to Levels[0]
			Bitmap image(fileName);
			Levels.SetSize(CeilLog2(Math::Max(image.GetWidth(), image.GetHeight())));
			Levels[0].Pixels.Reserve(image.GetWidth()*image.GetHeight());
			for (int i = image.GetHeight()-1; i>=0; i--)
				Levels[0].Pixels.AddRange((Color*)image.GetPixels() + i *image.GetWidth(), image.GetWidth());
			Levels[0].Width = image.GetWidth();
			Levels[0].Height = image.GetHeight();
			Width = image.GetWidth();
			Height = image.GetHeight();
			IsTransparent = image.GetIsTransparent();
			// Generate mipmaps
			int level = 0;
			int lwidth = Width, lheight = Height;
			InvWidth = 1.0f / Width;
			InvHeight = 1.0f / Height;
			do
			{
				int oldWidth = lwidth;
				int oldHeight = lheight;
				int oldLevel = level;
				level++;
				lwidth >>= 1;
				lheight >>= 1;
				if (lwidth == 0) lwidth = 1;
				if (lheight == 0) lheight = 1;
				Levels[level].Width = lwidth;
				Levels[level].Height = lheight;
				Levels[level].Pixels.SetSize(lwidth * lheight);
				for (int i = 0; i<lheight; i++)
				{
					int i1, i2;
					if (lheight < oldHeight)
					{
						i1 = i*2;
						i2 = i1 + 1;
					}
					else
					{

						i1 = i2 = i;
					}
					for (int j = 0; j<lwidth; j++)
					{
						int j1, j2;
						if (lwidth < oldWidth)
						{
							j1 = j*2;
							j2 = j1 + 1;
						}
						else
						{
							j1 = j2 = j;
						}
						Color c1,c2,c3,c4;
						c1 = Levels[oldLevel].Pixels[i1*oldWidth + j1];
						c2 = Levels[oldLevel].Pixels[i1*oldWidth + j2];
						c3 = Levels[oldLevel].Pixels[i2*oldWidth + j1];
						c4 = Levels[oldLevel].Pixels[i2*oldWidth + j2];
						Color c;
						c.R = (c1.R+c2.R+c3.R+c4.R)>>2;
						c.G = (c1.G+c2.G+c3.G+c4.G)>>2;
						c.B = (c1.B+c2.B+c3.B+c4.B)>>2;
						c.A = (c1.A+c2.A+c3.A+c4.A)>>2;
						Levels[level].Pixels[i*lwidth + j] = c;
					}
				}
			}
			while (lwidth != 1 || lheight != 1);
		}
	}
}