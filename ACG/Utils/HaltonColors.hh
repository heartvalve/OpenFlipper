//=============================================================================
//
//  CLASS HaltonColors (by Marcel Campen)
//
//=============================================================================


#ifndef ACG_HALTONCOLORS_HH
#define ACG_HALTONCOLORS_HH

//== INCLUDES =================================================================


#include <ACG/Math/VectorT.hh>


//== NAMESPACES ===============================================================

namespace ACG {

  //== CLASS DEFINITION =========================================================

	// Provides deterministic pseudo-random low-discrepancy colors with a
	// uniform distribution over a visually pleasing subset of HSL space,
    // independent of the number of colors required.
    // Simply instantiate and use get_next_color().
	class HaltonColors {

		int current[3]; // current Halton index
		int bases[3]; // Halton prime bases
		float inverse_bases[3];

	public:
		HaltonColors() {
			current[0] = 250; current[1] = 250; current[2] = 250; // skip first 250 sequence elements to lower discrepancy even further.
			bases[0] = 5; bases[1] = 13; bases[2] = 17; // initialize prime bases for H,S,L. Especially the first should be small such that already small numbers of generated colors are distributed over the whole color circle.
			inverse_bases[0] = 1.0/bases[0]; inverse_bases[1] = 1.0/bases[1]; inverse_bases[2] = 1.0/bases[2];
		}

	private:
		float halton(int index) {
		  int base = bases[index];
		  float inverse_base = inverse_bases[index];
		  float H = 0;
		  float half = inverse_base;
		  int I = current[index];
		  current[index] += 1;
		  while (I > 0)
		  {
			int digit = I%base;
			H = H + half*digit;
			I = inverse_base * (I-digit);
			half *= inverse_base;
		  }
		  return H;
		}

		float random_interval(int index, float min, float max) {
		    return halton(index) * (max-min) + min;
		}

	    ACG::Vec4f HSL2RGB(double h, double sl, double l) {
	        double v;
	        double r,g,b;

			r = l; g = l; b = l;

			v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);

			if (v > 0)
			{
				  double m;
				  double sv;
				  int sextant;
				  double fract, vsf, mid1, mid2;

				  m = l + l - v;
				  sv = (v - m ) / v;
				  h *= 6.0;
				  sextant = (int)h;
				  fract = h - sextant;
				  vsf = v * sv * fract;
				  mid1 = m + vsf;
				  mid2 = v - vsf;

				  switch (sextant)
				  {
						case 0:
							  r = v;
							  g = mid1;
							  b = m;
							  break;
						case 1:
							  r = mid2;
							  g = v;
							  b = m;
							  break;
						case 2:
							  r = m;
							  g = v;
							  b = mid1;
							  break;
						case 3:
							  r = m;
							  g = mid2;
							  b = v;
							  break;
						case 4:
							  r = mid1;
							  g = m;
							  b = v;
							  break;
						case 5:
							  r = v;
							  g = m;
							  b = mid2;
							  break;
				  }
			}

			return Vec4f(r, g, b, 1.0);
	    }


	public:
	    ACG::Vec4f get_next_color() {
		    float h = random_interval(0, 0, 0.9); // 0.9 instead of 1.0 to suppress natural bias towards red
		    float s = random_interval(1, 0.40, 0.80); // saturation between 40% and 80%
		    float l = random_interval(2, 0.30, 0.60); // lightness between 30% and 60%
		    return HSL2RGB(h, s, l);
		}
	};


  //=============================================================================
} 
//=============================================================================
#endif // ACG_HALTONCOLORS_HH defined
//=============================================================================

