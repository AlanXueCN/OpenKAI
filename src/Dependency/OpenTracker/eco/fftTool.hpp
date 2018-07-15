#ifndef FFTTOOL_HPP
#define FFTTOOL_HPP

#include <opencv2/imgproc/imgproc.hpp>

namespace eco
{
// Previous declarations, to avoid warnings
cv::Mat fftf(const cv::Mat &img_org, bool backwards = false);
cv::Mat fftd(const cv::Mat &img_org, bool backwards = false);
cv::Mat real(cv::Mat img);
cv::Mat imag(cv::Mat img);
cv::Mat magnitude(cv::Mat img);
cv::Mat complexMultiplication(cv::Mat a, cv::Mat b);
cv::Mat complexDivision(cv::Mat a, cv::Mat b);
void rearrange(cv::Mat &img);
void normalizedLogTransform(cv::Mat &img);

cv::Mat fftshift(const cv::Mat &org_img, bool rowshift = true, bool colshift = true, bool reverse = 0);
cv::Mat fftshiftd(const cv::Mat &org_img, bool rowshift = true, bool colshift = true, bool reverse = 0);
cv::Mat mat_conj(const cv::Mat &org);
float mat_sum(const cv::Mat &org); //** just for single channel float ***
double mat_sumd(const cv::Mat &org);

cv::Mat cmat_multi(const cv::Mat &a, const cv::Mat &b); //** the mulitiplciation of two complex matrix
cv::Mat real2complx(const cv::Mat &x);

inline bool SizeCompare(cv::Size &a, cv::Size &b) //** extra function for STL
{
	return a.height < b.height;
}

inline void rot90(cv::Mat &matImage, int rotflag)
{ //matrix rotation 1=CW, 2=CCW, 3=180

	if (rotflag == 1)
	{
		cv::transpose(matImage, matImage);
		cv::flip(matImage, matImage, 1); //transpose+flip(1)=CW
	}
	else if (rotflag == 2)
	{
		cv::transpose(matImage, matImage);
		cv::flip(matImage, matImage, 0); //transpose+flip(0)=CCW
	}
	else if (rotflag == 3)
	{
		cv::flip(matImage, matImage, -1); //flip(-1)=180
	}
	else if (rotflag != 0)
	{ //if not 0,1,2,3:
		assert("Unknown rotation flag");
	}
} //*** roa
//*** impliment matlab c = convn(a,b) no matter of real of complex, It can work
cv::Mat conv_complex(cv::Mat _a, cv::Mat _b, bool valid = 0); 

} // namespace eco

#endif
