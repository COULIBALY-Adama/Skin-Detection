// import packages
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

// Definition of the base path
#define NB_IMAGE 27
#define PATH_TO_SKIN_IMAGES "base/skin/"
#define PATH_TO_NO_SKIN_IMAGES "base/no-skin/"

using namespace std;
using namespace cv;

float** histogram(string type, int scale, float &nb_pixels) {

	float reduction_factor = (float) scale / 256;

	// Choice of image directory
	char* PATH;

	if (type.compare("skin") == 0) {
		PATH = PATH_TO_SKIN_IMAGES;
	} else if (type.compare("no_skin") == 0) {
		PATH = PATH_TO_NO_SKIN_IMAGES;
	} else {
		cout << "Directory error! You entered invalid skin";
	}

	// Creation of the matrix to contain the histogram
	float ** histogram;
	histogram = new float*[scale];
	for (int i = 0; i < scale; i++) {
		histogram[i] = new float[scale];
		for (int j = 0; j < scale; j++) {
			histogram[i][j] = 0;
		}
	}

	// Construction of the histogram
	for (int i = 1; i <= NB_IMAGE; i++) {

		// Definition of image name
		char image_name[50] = "";
		strcat(image_name, PATH);
		char num[2] = "";
		sprintf(num, "%d", i);
		strcat(image_name, num);
		strcat(image_name, ".jpg");

		// Loading image
		Mat image;
		image = imread(image_name, 1);

		if (!image.data) {
			cout << "Image is not valid" << endl;
			exit(0);
		} else {

			// Conversion in lab space
			Mat resultat;
			cvtColor(image, resultat, CV_BGR2Lab);

			// Path of the image to fill the histogram
			for (int k = 0; k < resultat.rows; k++) {
				for (int l = 0; l < resultat.cols; l++) {

					// choice of values a and b
					int a = resultat.at<Vec3b>(k, l).val[1]
							* reduction_factor;
					int b = resultat.at<Vec3b>(k, l).val[2]
							* reduction_factor;

					// update histogram values
					if (image.at<Vec3b>(k, l) != Vec3b(0, 0, 0)) {

						histogram[a][b] = histogram[a][b] + 1;
					}
				}
			}
		}
	}

	// Smoothing the histogram to improve detection:
			//average of the value of the 8 neighboring pixels + the value of the pixel

			for (int i = 1; i < (scale - 1); i++) {
				for (int j = 1; j < (scale - 1); j++) {
					histogram[i][j] = histogram[i][j]
							+ (histogram[i - 1][j - 1] + histogram[i - 1][j]
									+ histogram[i - 1][j + 1] + histogram[i][j - 1]
									+ histogram[i][j + 1] + histogram[i + 1][j - 1]
									+ histogram[i + 1][j] + histogram[i + 1][j + 1])
									/ 8;
				}
			}

	// Histogram normalization
	for (int m = 0; m < scale; m++) {
		for (int n = 0; n < scale; n++) {
			if(histogram[m][n] !=0)
				nb_pixels += histogram[m][n];

		}
	}

	for (int m = 0; m < scale; m++) {
			for (int n = 0; n < scale; n++) {
				if(histogram[m][n] !=0)
								histogram[m][n] /= nb_pixels;

			}
		}



	return histogram;
}

// Program performance evaluation

void evaluation(Mat image_reference, Mat image_detectee) {

	int nb_pixels_skin_true = 0;
	int nb_pixels_skin_false_pos = 0;
	int nb_pixels_skin_image_reference = 0;
	int nb_pixels_skin_false_neg = 0;
	float performance;

	for (int i = 0; i < image_detectee.rows; i++) {
		for (int j = 0; j < image_detectee.cols; j++) {

			Vec3b Resultat = image_detectee.at<Vec3b>(i, j);
			Vec3b original = image_reference.at<Vec3b>(i, j);
			// Number of skin pixels correctly detected in the result 
			// the pixel of the result image and the reference image are all different from black
			if (Resultat != Vec3b(0, 0, 0) && original != Vec3b(0, 0, 0)) {

				nb_pixels_skin_true++;
			}
			// Number of skin pixels incorrectly detected
			if (Resultat != Vec3b(0, 0, 0) && original == Vec3b(0, 0, 0)) {
				nb_pixels_skin_false_pos++;
			}
			// Number of skin pixels in the reference image
			if (original != Vec3b(0, 0, 0)) {
				nb_pixels_skin_image_reference++;
			}
		}
	}

	nb_pixels_skin_false_neg = nb_pixels_skin_image_reference -nb_pixels_skin_true;
	if(nb_pixels_skin_false_neg < 0.0)
		nb_pixels_skin_false_neg=0.0;

// Calculation of program performance
	performance = (float)nb_pixels_skin_true/(nb_pixels_skin_true
					+nb_pixels_skin_false_pos + nb_pixels_skin_false_neg);
	cout << "reference :"<<nb_pixels_skin_image_reference<< endl;
	cout << "correct :"<<nb_pixels_skin_true<< endl;
	cout << "false_positive :"<<nb_pixels_skin_false_pos<< endl;
	cout << "false_negative :"<<nb_pixels_skin_false_neg<< endl;

	cout << "Program performance = " << performance * 100 << " %" << endl;

}

// Simple method skin detection
Mat detection_skin_simple(float** histo_skin, float** histo_no_skin,
		Mat image_test, int scale) {

	float reduction_factor = (float) scale / 256;
	// conversion of the test image in the lab space
	Mat resultat;
	cvtColor(image_test, resultat, CV_BGR2Lab);

	Mat masque(image_test.rows, image_test.cols, CV_8UC1);
	masque = Scalar(0);
	Mat sortie;
	image_test.copyTo(sortie);
	for (int k = 0; k < resultat.rows; k++) {
		for (int l = 0; l < resultat.cols; l++) {

			// choice of values a and b
			int a = resultat.at<Vec3b>(k, l).val[1] * reduction_factor;
			int b = resultat.at<Vec3b>(k, l).val[2] * reduction_factor;

			//if(a!=0 || b!=0)
			//cout<< "a :"<< a << " b :"<< b << endl;
			// update histogram values 
			if (histo_skin[a][b] < histo_no_skin[a][b]) {

				sortie.at<Vec3b>(k, l) = Vec3b(0, 0, 0);

			} else {
				masque.at<uchar>(k, l) = 255;
			}
		}
	}

	imshow("input image", image_test);

	imshow("mask_image", masque);
	imshow("output_image", sortie);

	return sortie;
}

// Skin detection by probability calculation
Mat detection_skin_bayes(float** histo_skin, float** histo_no_skin,
		Mat image_test, int scale, float seuil, float nb_pixels_skin,
		float nb_pixels_no_skin) {

	float reduction_factor = (float) scale / 256;
	float proba_skin = 0.0;
	float proba_no_skin = 0.0;

	// calculation of skin and non-skin probabilities

	proba_skin = nb_pixels_skin / (nb_pixels_skin + nb_pixels_no_skin);
	proba_no_skin = nb_pixels_no_skin / (nb_pixels_skin + nb_pixels_no_skin);
//	cout << "nb skin :" << nb_pixels_skin << endl;
//	cout << "nb no skin :" << nb_pixels_no_skin << endl;
//	cout << "proba_skin :" << proba_skin << endl;
//	cout << "proba_no_skin :" << proba_no_skin << endl;

	// conversion of the test image in the lab spac
	Mat resultat;
	cvtColor(image_test, resultat, CV_BGR2Lab);

	// mask creation
	Mat masque(image_test.rows, image_test.cols, CV_8UC1);
	masque = Scalar(0);

	// creation of the result image
	Mat sortie;
	image_test.copyTo(sortie);

	for (int k = 0; k < resultat.rows; k++) {
		for (int l = 0; l < resultat.cols; l++) {

			// Choice of values a and b
			int a =0, b=0;
			 a = resultat.at<Vec3b>(k, l).val[1] * reduction_factor;
			 b = resultat.at<Vec3b>(k, l).val[2] * reduction_factor;
			 // Calculation of the decision probability
			 float proba_decision = 0.0;
			 proba_decision = (histo_skin[a][b] * proba_skin)
					/ (histo_skin[a][b] * proba_skin
							+ histo_no_skin[a][b] * proba_no_skin);

			// update histogram value
			if (proba_decision > seuil) {
				masque.at<uchar>(k, l) = 255;

			} else {
				sortie.at<Vec3b>(k, l) = Vec3b(0, 0, 0);
			}
		}

	}

	// Post treatment
	int erosion_size = 1;
	int dilatation_size = 3;

	Mat dilate_element = getStructuringElement(MORPH_CROSS,
			Size(2 * dilatation_size + 1, 2 * dilatation_size + 1),
			Point(dilatation_size, dilatation_size));

	Mat erode_element = getStructuringElement(MORPH_CROSS,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			Point(erosion_size, erosion_size));
	dilate(sortie, sortie, dilate_element);

	erode(sortie, sortie, erode_element);

	imshow("image input image", image_test);

	imshow("mask_image", masque);
	imshow("output_image", sortie);


	return sortie;

}

// Histogram display
void histogram_print(float ** histogram, int scale, string type) {

	Mat big_histogram(256, 256, CV_8UC1);
	float valeur_maximale = 0.0;

	// Determination of the maximum value of the histogram

	for (int i = 0; i < scale; i++) {
		for (int j = 0; j < scale; j++) {
			if (histogram[i][j] > valeur_maximale)
				valeur_maximale = histogram[i][j];
		}
	}

	// Enlargement, normalization of the histogram matrix and transformation into an image

	for (int i = 0; i < scale; i++) {
		for (int j = 0; j < scale; j++) {
			for (int k = 0; k < 256/scale; k++) {
				for (int l = 0; l < 256/scale; l++)
					big_histogram.at<uchar>(i * 256/scale + k, j * 256/scale + l) =
							saturate_cast<uchar>(
									((histogram[i][j]) / valeur_maximale)
											* 255);
						}
		}
	}

	// Histogram recordin
	char histogram_name[50] = "";
	strcat(histogram_name, "histogram/histogram");
	if (type.compare("skin") == 0) {
		strcat(histogram_name, "skin");
	} else {
		strcat(histogram_name, "no-skin");
	}
	strcat(histogram_name, ".jpg");
	if (!imwrite(histogram_name, big_histogram))
		cout << "Erreur lors de l'enregistrement" << endl;

	// Histogram display
	imshow(histogram_name, big_histogram);
}

// Main function
int main(int argc, char** argv) {

	int scale = 0;
	float seuil = 0.0;
	scale = atoi(argv[1]);
	seuil = atof(argv[2]);
	float ** histo_skin = NULL;
	float ** histo_no_skin = NULL;
	float nb_pixels_skin = 0;
	float nb_pixels_no_skin = 0;
	char* arg_nom = argv[3];
	char image_name_test[50]= "";
	strcat(image_name_test,"base/test/");
	strcat(image_name_test,arg_nom);

	// Reading the input image
	Mat image_entre;
	image_entre = imread(image_name_test, 1);

	char image_name_reference[50] = PATH_TO_SKIN_IMAGES;
	strcat(image_name_reference,arg_nom);

	// Reading the reference image
	Mat image_reference;
	image_reference = imread(image_name_reference, 1);
	imshow("image reference", image_reference);


	Mat image_detectee;

	// calculation of histograms
	histo_skin = histogram("skin", scale, nb_pixels_skin);

	histo_no_skin = histogram("no_skin", scale, nb_pixels_no_skin);

	// image_detectee = detection_peau_simple(histo_peau, histo_non_peau, image_entre, scale);

	image_detectee = detection_skin_bayes(histo_skin, histo_no_skin,
			image_entre, scale, seuil, nb_pixels_skin, nb_pixels_no_skin);

	char image_name_resultat[50] ="";
		strcat(image_name_resultat,"resultat/");
		strcat(image_name_resultat,"resultat_image_");
		strcat(image_name_resultat,arg_nom);
		if (!imwrite(image_name_resultat, image_detectee))
				cout << "Erreur lors de l'enregistrement" << endl;

	evaluation(image_reference, image_detectee);
	histogram_print(histo_skin,scale,"skin");
	histogram_print(histo_no_skin,scale,"no_skin");
	waitKey(0);
	return 0;
}
