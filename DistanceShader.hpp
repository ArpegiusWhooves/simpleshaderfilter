#ifndef DISTANCESHADER_H
#define DISTANCESHADER_H

using std::max;

// filtr odległości
class DistanceShader
{

	public:
		DistanceShader(int _color,float fallout = 1):
			delta(fallout),  //inicjacja mocy filtru
			color(_color)    //inicjacja koloru
		{

		}

	protected:

		enum
		{
			backbuf = 1, // backbuf=1 odwołujemy się jeden pixel do tyłu
			 dir = 0xF // dir = 0xF uruchamiamy filtr w 4 kierunki
		};


		struct TArgument   // argumenty filtru
		{
			TArgument():   //konstruktor domyślny argumentu
				maxa(0),
				dy(0),
				dx(0)
				{};
			float maxa;    // maxymalna wartość alpha
			float dy;     // odleglość y od maxymalnej wartości alpha
			float dx;    // odleglość x od maxymalnej wartości alpha
		};

		typedef Scanliner<DistanceShader> TShaderProcesor;  // używamy procesora Scanliner
		friend class Scanliner<DistanceShader>;				// deklarujemy jako przyjaciela
		friend class ShaderEngine;


		inline float getAlpha(float x, float y)   // zamiana współżędnych na wartość alpha
		{
			return hypot(x,y);
		}

		inline void eachpixel(TArgument *arg, int in, int &out)   // wywoływane przez Scanliner dla każdego pixla
		{

		    float a = 0;   // wyliczona wartość a

			if(arg->maxa > 0)  // jeżeli pixel w poprzedniej linijce  ma ustawiony maxymalna wartość
			{
				a = arg->maxa - getAlpha(arg->dx  , arg->dy+=delta);  // wylicz obecną wartość, współżędna dy zwięszona o 1
			}

			if((arg-1)->maxa > 0) // jeżeli pixel do tyłu ma ustawiony maxymalna wartość
			{
				float b = (arg-1)->maxa - getAlpha((arg-1)->dx+delta, (arg-1)->dy); // wylicz obecną wartość
				if(b > a)   // jeżeli jest ona większa od od poprzedniej
				{
					arg->dx = (arg-1)->dx+delta; // ustaw w aktualnej pozycji odleglość x od maxymalnej wartości alpha
					arg->dy = (arg-1)->dy;      // ustaw w aktualnej pozycji odleglość y od maxymalnej wartości alpha
					arg->maxa = (arg-1)->maxa; // ustaw w aktualnej pozycji maxymalna wartość alpha
					a = b; // ustaw na większą
				}
			}

			if( a <= 0 ) a = 0;  // niemiejsze od 0


			using std::max;

			int na = geta(in) > 128 ? 255 : 0; // odczytaj wartość alpha z wejścia

			if( na >= a ){  // jeżeli na wejściu jest większa wartość niż wyliczona
				//out = color | (max(((unsigned int)out)&0xFF000000,((unsigned int)in)&0xFF000000));
				arg->maxa = na;  // ustaw ją jakomaxymalną
				arg->dx = 0;  // resetuj odległość x
				arg->dy = 0;  // resetuj odległość y
				a = na;
			}

			out = color | // usaw kolor na wyjściu
					max(  // ale nie mniejszy niż obecny na wyjściu
						((unsigned int)out)&0xFF000000,
						((unsigned int) a) << 24
					);

		}

	private:
		float delta;  // moc filtru
		unsigned int color; // moc filtru
};



#endif // DISTANCESHADER_H
