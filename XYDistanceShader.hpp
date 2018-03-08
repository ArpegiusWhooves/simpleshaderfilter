#ifndef XYDISTANCESHADER_H
#define XYDISTANCESHADER_H

#include "Scanliner.hpp"

class XYDistanceShader
{
	public:
		XYDistanceShader(int _color,float fallout = 1):
			lasta(0),
			color(_color),
			dx(fallout)
		{

		}

	protected:
		enum{ backbuf = 1, dir = 0xF };

		struct TArgument {
			TArgument():a(0),lasta(0){};
			float a;
			float lasta;
		};

		typedef Scanliner<XYDistanceShader> TShaderProcesor;
		friend class Scanliner<XYDistanceShader>;
		friend class ShaderEngine;

		void eachpixel(TArgument *arg, int in, int &out)
		{
			using std::max;
			using std::min;

			float a = geta(in);
			arg->a = min(
						max(
							max(arg->a, (arg-1)->a)-dx,
							a
						) +
						max(
							std::abs(arg->lasta-a),
							std::abs((arg-1)->lasta-a)
						),
						255.f);
			out = color | (max(out&0xFF000000,((unsigned int) arg->a) << 24) );
			arg->lasta=a;
		}

	private:
		int lasta;
		int color;
		float dx;
};



#endif // XYDISTANCESHADER_H
