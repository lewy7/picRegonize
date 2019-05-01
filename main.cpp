#include "cv.h"
#include "highgui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

IplImage* img = 0;

//金字塔采样 
void pyramid()
{
	IplImage* img = cvLoadImage("c:\\84.jpg", 1 );
	CvSize sz = cvSize( img->width & -2, img->height & -2 );
	IplImage* pyrD = cvCreateImage(cvSize(sz.width/2, sz.height/2), 8, 3 );
	IplImage* pyrU = cvCreateImage(sz, 8, 3 );
	cvNamedWindow( "orgin", 1 );cvShowImage("orgin",img);
	cvPyrDown( img, pyrD, 7 );//输出图像尺寸减半
	cvNamedWindow( "up", 1 );cvShowImage("up",pyrD);
    cvPyrUp( pyrD, pyrU, 7 );//输出图像尺寸加倍
	cvNamedWindow( "down", 1 );cvShowImage("down",pyrU);

	cvWaitKey();

	cvReleaseImage( &img );
	cvReleaseImage( &pyrD );
	cvReleaseImage( &pyrU );
	cvDestroyWindow( "orgin" );
	cvDestroyWindow( "up" );
	cvDestroyWindow( "down" );
}

//通道输出
void channel()
{
	IplImage* img = cvLoadImage("c:\\84.jpg", 1 );
	IplImage* r = cvCreateImage(cvSize(img->width,img->height), 8, 1 );
	IplImage* g = cvCreateImage(cvSize(img->width,img->height), 8, 1 );
	IplImage* b = cvCreateImage(cvSize(img->width,img->height), 8, 1 );

	cvSetImageCOI( img, 1);
	cvCopy( img, r, 0 );cvNamedWindow( "r", 1 );cvShowImage("r",r);
	cvSetImageCOI( img, 2);
	cvCopy( img, g, 0 );cvNamedWindow( "g", 1 );cvShowImage("g",g);
	cvSetImageCOI( img, 3);
	cvCopy( img, b, 0 );cvNamedWindow( "b", 1 );cvShowImage("b",b);

	cvWaitKey();
	
	cvReleaseImage( &img );
	cvReleaseImage( &r );
	cvReleaseImage( &g );
	cvReleaseImage( &b );
	cvDestroyWindow( "r" );
	cvDestroyWindow( "g" );
	cvDestroyWindow( "b" );

}

//边缘查找
void canny()
{
	IplImage* img = cvLoadImage("c:\\car.jpg", 1 );
	IplImage* gray = cvCreateImage(cvSize(img->width,img->height), 8, 1 );
	IplImage* edge = cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 1);

	cvCvtColor(img, gray, CV_BGR2GRAY);
	cvNamedWindow( "gray", 1 );cvShowImage("gray",gray);

	cvSmooth(gray, edge, CV_BLUR, 3, 3, 0, 0 );
	float edge_thresh  = 45;
	cvCanny(gray, edge, edge_thresh, edge_thresh*3, 3);
	cvDilate( edge,edge, cvCreateStructuringElementEx( 10*2+1, 10*2+1, 10, 10, CV_SHAPE_RECT, 0 ), 1 );
    cvNamedWindow( "cvCanny", 1 );cvShowImage("cvCanny", edge);

	cvWaitKey(0);
    cvReleaseImage(&img);
    cvReleaseImage(&gray);
    cvReleaseImage(&edge);
    cvDestroyWindow("gray");

}

//腐蚀和膨胀
void erode_dilate()
{
	IplImage* img = cvLoadImage("c:\\id.jpg", 1 );
	IplImage* erode = cvCloneImage(img);
	IplImage* dilate = cvCloneImage(img);
	int an = 20;
	//CV_SHAPE_RECT  CV_SHAPE_ELLIPSE   CV_SHAPE_CROSS
	IplConvKernel* element = cvCreateStructuringElementEx( an*2+1, an*2+1, an, an, CV_SHAPE_RECT, 0 );
	cvErode(img,erode,element,1);
	cvNamedWindow( "erode", 1 );cvShowImage("erode", erode);
	cvDilate(img,dilate,element,1);
	cvNamedWindow( "dilate", 1 );cvShowImage("dilate", dilate);

	cvWaitKey();
	cvReleaseStructuringElement(&element);
	cvReleaseImage(&img);
	cvReleaseImage(&erode);
    cvReleaseImage(&dilate);
	cvDestroyWindow("erode");
	cvDestroyWindow("dilate");
} 

//伐值
void threshold()
{
	IplImage* gray = cvLoadImage( "c:\\84.jpg", 0 );
	IplImage* threshold = cvCloneImage( gray );
	float edge_thresh  = 70;
	cvThreshold( gray, threshold, (float)edge_thresh, (float)edge_thresh, CV_THRESH_BINARY );
	cvNamedWindow( "threshold", 1 );cvShowImage("threshold", threshold);
	cvWaitKey();
	cvReleaseImage(&gray);
	cvReleaseImage(&threshold);
	cvDestroyWindow("threshold");
}

//轮廓
void findContours()
{
	IplImage* img = cvLoadImage("c:\\id.jpg", 1);
	IplImage* gray = cvCreateImage(cvSize(img->width,img->height), 8, 1 );
	IplImage* output = cvCloneImage( img );
	CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contour = NULL;
	
	
	int slider_pos = 50, an = 10;
	cvCvtColor (img, gray, CV_BGR2GRAY);
	cvThreshold( gray, gray, slider_pos, 255, CV_THRESH_BINARY );
	IplConvKernel* element = cvCreateStructuringElementEx( an*2+1, an*2+1, an, an, CV_SHAPE_RECT, 0 );
	cvErode(gray,gray,element,1);
	cvNamedWindow( "gray", 1 );cvShowImage("gray", gray);cvWaitKey();
	cvFindContours( gray, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE, cvPoint(0,0));

	CvPoint min, max;
	CvPoint* p;
	int i,width,height;
	CvRect rect = cvRect(0,0,0,0);
	for( ; contour != 0; contour = contour->h_next ){
		//转换成多边形
		contour = cvApproxPoly( contour, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 3, 1 );

		//丢弃复杂的多边形
		if(contour->total<4 || contour->total>100){
			cvClearSeq(contour);
			continue;
		}
		//将多边形转成垂直四边形
		min.x = 99999; min.y = 99999; max.x = 0; max.y = 0;
		for(i = 0; i<contour->total; i++){
			p = (CvPoint *)cvGetSeqElem(contour,i);
			min.x = p->x < min.x  ?  p->x : min.x;
			min.y = p->y < min.y  ?  p->y : min.y;
			max.x = p->x > max.x  ?  p->x : max.x;
			max.y = p->y > max.y  ?  p->y : max.y;
		}
		width = max.x - min.x;
		height = max.y - min.y;
		if(width/3 < height){ //长宽比判断
			continue;
		}
		if(rect.x+rect.y<min.x+min.y){ //最左下角判断
			printf(" reg %d %d  %d rect add  %d %d  %d\n",rect.x,rect.y,rect.x+rect.y,max.x,max.y,max.x+max.y);
			rect = cvRect(min.x,min.y,width,height);
			
		}
		printf("%d %d %d %d : %d %d  :%d\n",min.x,min.y,max.x,max.y,  width,height,max.x+max.y);
		
		cvRectangle( output,cvPoint(min.x,min.y), cvPoint(max.x,max.y), CV_RGB(255,0,255), 1, 8, 0 );
        
// 		rect[0].x = min.x;rect[0].y = min.y;
// 		rect[1].x = min.x;rect[1].y = min.y;
// 		rect[2].x = min.x;rect[2].y = min.y;
// 		rect[3].x = min.x;rect[3].y = min.y;
// 		p = rect;
// 		cvPolyLine( output, &p, &count, 1, 1, CV_RGB(0,255,0), 3, CV_AA, 0 );
   
		
		//cvDrawContours (output, contour, CV_RGB(255,0,0),CV_RGB(0,0,100),1,2,8,cvPoint(0,0));
		//for(int i = 0; i<result->total; i++)
		//{
		//	CvPoint* p = (CvPoint *)cvGetSeqElem(result,i);
		//	printf("(%d,%d) ",p->x,p->y);
		//}
		//printf("\n---------------------------------------\n");
	}
	printf("rect %d %d  %d\n",rect.x,rect.y,height,rect.x+rect.y);
	cvSetImageROI(output,rect);
	cvSaveImage("c:\\ouput.jpg", output);
	cvResetImageROI(output);

// 	for(;contour;contour = contour->h_prev){
// 		printf("\n contour->total %d contour->h_prev %p --\n",contour->total,contour->h_prev);
// 		for(int i = 0; i<contour->total; i++){
// 			CvPoint* p = (CvPoint *)cvGetSeqElem(contour,i);
// 			//printf("(%d,%d) ",p->x,p->y);
// 		}
// 		
// 	}

	 
	cvDrawContours (output, contour, CV_RGB(255,0,0),CV_RGB(0,0,100),1,2,8,cvPoint(0,0));
	cvNamedWindow( "findContours", 1 );cvShowImage("findContours", output); cvWaitKey();
	cvReleaseImage(&img);
	cvReleaseImage(&output);
	cvDestroyWindow("findContours");
}

int main()
{
	//pyramid(); 
	//channel();
	//canny();
	//erode_dilate();
	//threshold();
	findContours();
	return 0;
}
