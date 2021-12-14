#ifndef CORE_LIB_MATH_H
#define CORE_LIB_MATH_H

namespace CoreLib
{
	namespace Basic
	{
		class Math
		{
		public:
			static const float Pi;
			template<typename T>
			static T Min(const T& v1, const T&v2)
			{
				return v1<v2?v1:v2;
			}
			template<typename T>
			static T Max(const T& v1, const T&v2)
			{
				return v1>v2?v1:v2;
			}
			template<typename T>
			static T Clamp(const T& val, const T& vmin, const T&vmax)
			{
				if (val < vmin) return vmin;
				else if (val > vmax) return vmax;
				else return val;
			}

			static inline unsigned int Ones32(register unsigned int x)
			{
				/* 32-bit recursive reduction using SWAR...
					but first step is mapping 2-bit values
					into sum of 2 1-bit values in sneaky way
				*/
				x -= ((x >> 1) & 0x55555555);
				x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
				x = (((x >> 4) + x) & 0x0f0f0f0f);
				x += (x >> 8);
				x += (x >> 16);
				return(x & 0x0000003f);
			}

			static inline unsigned int Log2Floor(register unsigned int x)
			{
				x |= (x >> 1);
				x |= (x >> 2);
				x |= (x >> 4);
				x |= (x >> 8);
				x |= (x >> 16);
				return(Ones32(x >> 1));
			}

			static inline unsigned int Log2Ceil(register unsigned int x)
			{
				register int y = (x & (x - 1));
				y |= -y;
				y >>= (32 - 1);
				x |= (x >> 1);
				x |= (x >> 2);
				x |= (x >> 4);
				x |= (x >> 8);
				x |= (x >> 16);
				return(Ones32(x >> 1) - y);
			}
			/*static inline int Log2(float x)
			{
				unsigned int ix = (unsigned int&)x;
				unsigned int exp = (ix >> 23) & 0xFF;
				int log2 = (unsigned int)(exp) - 127;

				return log2;
			}
			*/
		};
		inline int FloatAsInt(float val)
		{
			union InterCast
			{
				float fvalue;
				int ivalue;
			} cast;
			cast.fvalue = val;
			return cast.ivalue;
		}
		inline float IntAsFloat(int val)
		{
			union InterCast
			{
				float fvalue;
				int ivalue;
			} cast;
			cast.ivalue = val;
			return cast.fvalue;
		}
	}
}

#endif 
