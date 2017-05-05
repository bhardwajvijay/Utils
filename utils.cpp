#include "utils.h"

// Perform h maxima transform
cv::Mat imhmax(cv::Mat im, double thresh){
	im.convertTo(im, CV_8UC1);	
	cv::Mat im_out = imreconstruct(im - thresh, im);		
	return im_out;
}

// Perform h minima transform
cv::Mat imhmin(cv::Mat im, double thresh){
	im.convertTo(im, CV_8UC1);
	im = 255 - im;
	im = imreconstruct(im - thresh, im);
	cv::Mat im_out = 255 - im;
	return im_out;
}

// imreconstruct
cv::Mat imreconstruct(cv::Mat marker, cv::Mat mask){
	// Structuring element
	cv::Mat se = cv::Mat::ones(3, 3, CV_32F);
	bool isDone = false;
	while(!isDone){
		cv::Mat marker_dil;
		dilate(marker, marker_dil, se);
		cv::Mat im_min = cv::min(marker_dil, mask);
		cv::Mat dst;
		cv::bitwise_xor(im_min, marker, dst);        
		if(cv::countNonZero(dst) > 0){
			marker = im_min;			
		}			
		else{
			isDone = true;
		}
	}
	return marker;
}

// By default connectivity is 8
cv::Mat imregionalmin(cv::Mat img){
	cv::Mat img_temp = img.clone();

	// Complement image
	img_temp = 255 - img_temp;
	cv::Mat recon = imreconstruct(img_temp, img_temp + 1);
	cv::Mat bw = cv::Mat::zeros(recon.rows, recon.cols, CV_8UC1);
	bw.setTo(255, recon == img_temp);
	return bw;
}

// Correct Gamma
cv::Mat correctGamma( cv::Mat& img, double gamma ) {
	double inverse_gamma = 1.0 / gamma;

	cv::Mat lut_matrix(1, 256, CV_8UC1 );
	uchar * ptr = lut_matrix.ptr();
	for( int i = 0; i < 256; i++ )
		ptr[i] = (int)( pow( (double) i / 255.0, inverse_gamma ) * 255.0 );

	cv::Mat result;
	LUT( img, lut_matrix, result ); 
	return result;
}

// Finding number of files in a folder
int findNumberofFiles(const wchar_t* file){
	bool x = true;
	int i = 0;
	//wchar_t* file = L"d:/testing/*.txt";
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	hFind = FindFirstFile(file, &FindFileData); 

	if( hFind != INVALID_HANDLE_VALUE ) {
		i++;
		while ((x = FindNextFile(hFind, &FindFileData)) == TRUE)
			i++; 
	}
	return i;
}

// Opening by reconstuction
cv::Mat opening_reconstruct(cv::Mat im, int sz){	
	cv::Mat se = cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(sz, sz));
	cv::Mat Ie;
	erode(im, Ie, se);
	cv::Mat Iobr = imreconstruct(Ie, im);
	return Iobr;
}

// Closing by reconstruction
cv::Mat closing_reconstruct(cv::Mat im, int sz){	
	cv::Mat se = cv::getStructuringElement(MORPH_ELLIPSE, cv::Size(sz, sz));
	cv::Mat Id;
	dilate(im, Id, se);
	cv::Mat Icbr = imreconstruct(Id, im);	
	return Icbr;
}