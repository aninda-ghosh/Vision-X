///  @function Set the camera property
void setcam(int camno1,int camno2)
{
    capture1 = cvCaptureFromCAM(camno1);
    capture2 = cvCaptureFromCAM(camno2);

    cvSetCaptureProperty(capture1, CV_CAP_PROP_FRAME_WIDTH, 320);
	cvSetCaptureProperty(capture1, CV_CAP_PROP_FRAME_HEIGHT, 240);

	cvSetCaptureProperty(capture2, CV_CAP_PROP_FRAME_WIDTH, 320);
	cvSetCaptureProperty(capture2, CV_CAP_PROP_FRAME_HEIGHT, 240);
}

///  @function Retrieve the image
IplImage *image_retrieve(int camno)
{
    if(camno == 1)
	{
        if (!cvGrabFrame(capture1))
        {
            cout << "Could not grab frame 1\n\7" << endl;
        }
        image = cvRetrieveFrame(capture1);
        return image;
    }
    else if(camno == 2)
    {
        if (!cvGrabFrame(capture2))
        {
            cout << "Could not grab frame 2\n\7" << endl;
        }
        image = cvRetrieveFrame(capture2);
        return image;
    }
}

///  @function get the image data
void image_data(IplImage *im)
{
	height = im->height;
	width = im->width;
	step = im->widthStep;
	channels = im->nChannels;
}

///  @function convert to matrix
Mat ConvertToMatrix(IplImage *im)
{
	Mat mat = cvarrToMat(im);
	return mat;
}

///  @function converting the rgb value to gray level
Mat convertToGray(Mat m1)
{
	cvtColor(m1, m1, CV_BGR2GRAY);
	return m1;
}

/** @function Generating required matrices */
void GenerateMatrices()
{
    OrigIm = ConvertToMatrix(img1);  ///Original image converted into matrix form
    GndIm = ConvertToMatrix(img2);  ///Original image converted into matrix form
    HumanIm = OrigIm.clone();
}





/*********************************************************Segmentation algorithm******************************************************************/
/** @function Modified K-Means */
Mat Modified_Kmeans(Mat orig,int *histarray,int ClusterNo)///8-bit Histogram Array
{
    /******Modified K-Means ALGORITHM*******
    1.  Initialization: distribute the K − 1 thresholds over the histogram. (For example in
        such a way that the grey value range is divided into K pieces of equal length.)
        Segment the image according to the thresholds set. For each segment, compute
        the ’cluster center’,i.e., the value midway between the two thresholds that make
        up the segment.
    2.  For each segment, compute the mean pixel value μ i .
    3.  Reset the cluster centers to the computed values μ i .
    4.  Reset the thresholds to be midway between the cluster centers, and segment the
        image.
    5.  Go to step 2. Iterate until the cluster centers do not move anymore (or do not move
        significantly).
    **/

    Mat finalgray = convertToGray(orig);
    Mat GrayIm = convertToGray(orig);
    range = (int)(256 / ClusterNo); ///Finding the equivalent range for each cluster

    /** Initialisation of cluster centres **/
    ClusterCenter = new int[ClusterNo];         ///Setting k cluster points
    PrevClusterCenter = new int[ClusterNo];     ///Setting k prev cluster points
    ThresholdPoints = new int[ClusterNo - 1];   ///Setting k-1 threshold points

    for(i=0; i<ClusterNo-1; i++)  ///Setting the cluster initial mean and the threshold points
    {
        ThresholdPoints[i] = range + (i*range);
    }

    for(i=0; i<ClusterNo; i++)
    {
        if(i==0)
        {
            ClusterCenter[i] = (0 + ThresholdPoints[i])/2;  ///Generating the Leftmost ClusterCenter points
        }
        else if(i==(ClusterNo-1))
        {
            ClusterCenter[i] = (ThresholdPoints[i-1]+255)/2;  ///Generating the Rightmost ClusterCenter points
        }
        else
        {
            ClusterCenter[i] = (ThresholdPoints[i-1]+ThresholdPoints[i])/2; ///Generating the Middle ClusterCenter points
        }
        PrevClusterCenter[i] = 0;
    }

    do
    {
        ///Calculating the mean of the cluster (ie updating the cluster centers)
        sigmanum = 0; ///Sigma (Intensity * No of pixels )
        sigmaden = 0; ///Sigma (Intensity)
        total_var = 0; ///resetting the global varience

        for(i=0; i<ClusterNo; i++)
        {
            sigmanum = 0; ///Sigma (Intensity * No of pixels )
            sigmaden = 0; ///Sigma (Intensity)
            if(i==0)
            {
                for(j=0; j<ThresholdPoints[i]; j++)
                {
                    sigmanum = sigmanum + histarray[j] * (j+1);
                    sigmaden = sigmaden + histarray[j];
                }
            }
            else if(i == (ClusterNo-1))
            {
                for(j=ThresholdPoints[i-1]; j<256; j++)
                {
                    sigmanum = sigmanum + histarray[j] * (j+1);
                    sigmaden = sigmaden + histarray[j];
                }
            }
            else
            {
                for(j=ThresholdPoints[i-1]; j<ThresholdPoints[i]; j++)
                {
                    sigmanum = sigmanum + histarray[j] * (j+1);
                    sigmaden = sigmaden + histarray[j];
                }
            }

            if(sigmaden == 0)
            {
                ClusterCenter[i] = PrevClusterCenter[i];
            }
            else
            {
                ClusterCenter[i] = sigmanum / sigmaden; ///sigma Intensity * no of pixels/ sigma intensity
            }
        }
        for(i=0; i<ClusterNo-1; i++)
        {
            ThresholdPoints[i] = (ClusterCenter[i]+ClusterCenter[i+1])/2; ///Generating the Middle Threshold points
        }
        for(i=0; i<ClusterNo; i++)
        {
            total_var = total_var + ((ClusterCenter[i] - PrevClusterCenter[i]) * (ClusterCenter[i] - PrevClusterCenter[i])) ;
        }
        for(i=0; i<ClusterNo; i++)
        {
            PrevClusterCenter[i] = ClusterCenter[i];
        }
        global_var = sqrt(total_var);

    }while(global_var > threshold_var);

    ///Setting the colours for different regions
    for (i = 1; i < height; i++)
	{
		for (j = 1; j < width; j++)
		{
            if(ClusterNo > 2)
            {
                if((int)GrayIm.at<uchar>(i,j) > 0 && (int)GrayIm.at<uchar>(i,j) < ThresholdPoints[0]) ///COLOR FOR THE EXTREME LEFT CLUSTER
                {
                    finalgray.at<uchar>(i,j) = ClusterCenter[0];
                }
                else if((int)GrayIm.at<uchar>(i,j) > ThresholdPoints[ClusterNo - 2] && (int)GrayIm.at<uchar>(i,j) < 256)
                {
                    finalgray.at<uchar>(i,j) = ClusterCenter[ClusterNo - 1];
                }
                else
                {
                    for(k=1;k<ClusterNo-1;k++)
                    {
                        if((int)GrayIm.at<uchar>(i,j) > ThresholdPoints[k-1] && (int)GrayIm.at<uchar>(i,j) <  ThresholdPoints[k])
                        {
                            finalgray.at<uchar>(i,j) = ClusterCenter[k];
                        }
                    }
                }
            }
            else
            {
                if((int)GrayIm.at<uchar>(i,j) > 0 && (int)GrayIm.at<uchar>(i,j) < ThresholdPoints[0]) ///COLOR FOR THE EXTREME LEFT CLUSTER
                {
                    finalgray.at<uchar>(i,j) = ClusterCenter[0];
                }
                else if((int)GrayIm.at<uchar>(i,j) > ThresholdPoints[ClusterNo - 2] && (int)GrayIm.at<uchar>(i,j) < 256)
                {
                    finalgray.at<uchar>(i,j) = ClusterCenter[ClusterNo - 1];
                }
            }
        }
    }

    return  finalgray;
}





/********************************************************Morphological Operations*******************************************************************
/* @function Lowpass filter */
Mat smoothing(Mat smoothimage)
{
    Mat LowPassIm = smoothimage.clone();     ///Low Pass Filtered image
    ///Complete the task (Gaussian Smoothing out technique)
    for(i = 1; i < height; i++)
    {
        for(j = 1; j < width; j++)
        {
            LowPassIm.at<uchar>(i,j) = ((41*(int)smoothimage.at<uchar>(i,j)) + (26*((int)smoothimage.at<uchar>(i-1,j)+(int)smoothimage.at<uchar>(i,j-1)+(int)smoothimage.at<uchar>(i+1,j)+(int)smoothimage.at<uchar>(i,j+1))) + (16*((int)smoothimage.at<uchar>(i-1,j-1)+(int)smoothimage.at<uchar>(i+1,j+1)+(int)smoothimage.at<uchar>(i+1,j-1)+(int)smoothimage.at<uchar>(i-1,j+1))) + (4*((int)smoothimage.at<uchar>(i-1,j-2)+(int)smoothimage.at<uchar>(i+1,j-2)+(int)smoothimage.at<uchar>(i+2,j-1)+(int)smoothimage.at<uchar>(i+2,j+1)+(int)smoothimage.at<uchar>(i-1,j+2)+(int)smoothimage.at<uchar>(i+1,j+2)+(int)smoothimage.at<uchar>(i-2,j-1)+(int)smoothimage.at<uchar>(i-2,j+1))) + (7*((int)smoothimage.at<uchar>(i,j-2)+(int)smoothimage.at<uchar>(i+2,j)+(int)smoothimage.at<uchar>(i,j+2)+(int)smoothimage.at<uchar>(i-2,j))) + (1*((int)smoothimage.at<uchar>(i-2,j-2)+(int)smoothimage.at<uchar>(i+2,j+2)+(int)smoothimage.at<uchar>(i+2,j-2)+(int)smoothimage.at<uchar>(i-2,j+2))))/273;
        }
    }
    //imshow("Low Pass Image",LowPassIm);
    return LowPassIm;
}

/** @function Differential smoothing */
Mat differentialsmoothing(Mat processed,Mat Lowpass)
{
    Mat low1= processed.clone();
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            if(i==0 || i==1 || i==2 || j==0 || j==1 || j==2 || i==319 || i==318 || i==317 || j==239 || j==238 || j==237 )
            {
                low1.at<uchar>(i,j) = 0;
            }
            else
            {
                low1.at<uchar>(i,j) = processed.at<uchar>(i,j) - Lowpass.at<uchar>(i,j);
            }
        }
    }

    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            if((int)low1.at<uchar>(i,j) >180)
            {
                low1.at<uchar>(i,j) = 255;
            }
            else
            {
                low1.at<uchar>(i,j) = 0;
            }
        }
    }

    return low1;
}

/**  @function Erosion  */
Mat Erosion(Mat src,int erosion_size,int e_type,int operation)
{
    int erosion_type;
    int horizontalsize,verticalsize;
    if(e_type == 1) erosion_type = MORPH_RECT;
    else if(e_type == 2) erosion_type = MORPH_CROSS;
    else if(e_type == 3) erosion_type = MORPH_ELLIPSE;
    else if(e_type == 4) erosion_type = MORPH_RECT;

    Mat erosion_dst;
    if(e_type == 1 || e_type == 2 ||e_type == 3)
    {
         Mat element;
        element= getStructuringElement( erosion_type,Size( 2*erosion_size + 1, 2*erosion_size+1 ),Point( erosion_size, erosion_size ) );
        erode( src, erosion_dst,element);
    }
    else if(e_type == 4)
    {
        Mat Horizontal = src.clone();
        Mat Vertical = src.clone();
        horizontalsize = erosion_size;
        verticalsize = erosion_size;

        Mat Hor_Struc = getStructuringElement( erosion_type,Size(horizontalsize,1));
        Mat Ver_Struc = getStructuringElement( erosion_type,Size(1,verticalsize));

        erode( Horizontal,Horizontal,Hor_Struc,Point(-1,-1));
        erode( Vertical,Vertical,Ver_Struc,Point(-1,-1));

        if(operation == 1)
        {
            erosion_dst = Vertical.clone();
        }
        else if (operation == 2)
        {
            erosion_dst = Horizontal.clone();
        }
    }
    return erosion_dst;
}

/** @function Dilation */
Mat Dilation(Mat src,int dilation_size,int d_type,int operation)
{
    int dilation_type;
    int horizontalsize,verticalsize;
    if(d_type == 1) dilation_type = MORPH_RECT;
    else if(d_type == 2) dilation_type = MORPH_CROSS;
    else if(d_type == 3) dilation_type = MORPH_ELLIPSE;
    else if(d_type == 4) dilation_type = MORPH_RECT;

    Mat dilation_dst;
    if(d_type == 1 || d_type == 2 ||d_type == 3)
    {
        Mat element;
        element= getStructuringElement( dilation_type,Size( 2*dilation_size + 1, 2*dilation_size+1 ),Point( dilation_size, dilation_size) );
        dilate( src, dilation_dst,element);
    }
    else if(d_type == 4)
    {
        Mat Horizontal = src.clone();
        Mat Vertical = src.clone();
        horizontalsize = dilation_size;
        verticalsize = dilation_size;

        Mat Hor_Struc = getStructuringElement( dilation_type,Size(horizontalsize,1));
        Mat Ver_Struc = getStructuringElement( dilation_type,Size(1,verticalsize));

        dilate( Horizontal,Horizontal,Hor_Struc,Point(-1,-1));
        dilate( Vertical,Vertical,Ver_Struc,Point(-1,-1));

        if(operation == 1)
        {
            dilation_dst = Vertical.clone();
        }
        else if (operation == 2)
        {
            dilation_dst = Horizontal.clone();
        }
    }
    return dilation_dst;
}

/*************************************************************Image features************************************************************************/
/** @function Histogram generation */
int *histogram(Mat OIM)
{
    int temp;
    int *HistNo;
    Mat GrayIm = convertToGray(OIM);
    Mat HistIm = OIM.clone();///Histogram representation on the blank window
    int maxi,res;
    HistNo = new int[256];          ///Array containing the pixel amount i numbers
    for (i = 0; i < height; i++)    ///getting a white screen
    {
        for (j = 0; j < width; j++)
        {
            HistIm.at<Vec3b>(i,j)[0] = 255;
            HistIm.at<Vec3b>(i,j)[1] = 255;
            HistIm.at<Vec3b>(i,j)[2] = 255;
        }
    }
    for(i=0;i<256;i++)
    {
        HistNo[i] = 0;      ///Setting the gray values which do not have any corresponding pixels to 0
    }
    for (i = 0; i < height; i++)    ///Generating the Histogram computationally
    {
        for (j = 0; j < width; j++)
        {
            temp = (int)GrayIm.at<uchar>(i,j);  ///temp image storing the gray value and incrementing the count where the gray value satisfies in the array
            HistNo[temp]++;
        }
    }
    maxi = maximum(HistNo,256);     ///Getting the maximum number of pixel points for a particular gray value
    res = HistNo[maxi]/200;         ///Generating the resolution of a per pixel representation
    //cout<<"Highest pixel count->"<<HistNo[maxi]<<" Resolution->"<<res<<endl;

    for (j = 32; j < 288; j++)    ///Shading technique
    {
        for (i = 0; i < 240; i++)
        {
            HistIm.at<Vec3b>(i,j)[0] = j-32;
            HistIm.at<Vec3b>(i,j)[1] = j-32;
            HistIm.at<Vec3b>(i,j)[2] = j-32;
        }
    }
    for (j = 32; j < 288; j++)    ///Generating the Histogram graphically
    {
        for (i = 220; i > 220-(HistNo[j-32]/res); i--)    ///Check the number of pixels and plot graphically
        {
            HistIm.at<Vec3b>(i,j)[0] = 0;
            HistIm.at<Vec3b>(i,j)[1] = 255;
            HistIm.at<Vec3b>(i,j)[2] = 0;
        }
    }
    for (i = 0; i < height; i++)    ///Sidepanel leftside
    {
        for (j = 0; j < 32; j++)
        {
            HistIm.at<Vec3b>(i,j)[0] = 0;
            HistIm.at<Vec3b>(i,j)[1] = 0;
            HistIm.at<Vec3b>(i,j)[2] = 0;
        }
    }
    for (i = 0; i < height; i++)    ///Sidepanel rightside
    {
        for (j = 288; j < 320; j++)
        {
            HistIm.at<Vec3b>(i,j)[0] = 255;
            HistIm.at<Vec3b>(i,j)[1] = 255;
            HistIm.at<Vec3b>(i,j)[2] = 255;
        }
    }
    //imshow("Histogram Image",HistIm);   ///Generating the Histogram of the image

    return HistNo;  ///Returning Histogram Array
}





/*****************************************************************Analysis***********************************************************************/
bool humansense( Mat frame )
{
    std::vector<Rect> faces;
    std::vector<Rect> eyes;
    Mat frame_gray;
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    int face_count=0;
    ///equalizeHist( frame_gray, frame_gray );

    ///-- Detect faces
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
    if(faces.size())
    {
        for( size_t i = 0; i < faces.size(); i++ )
        {
            Point center( faces[i].x + faces[i].width*0.7, faces[i].y + faces[i].height*0.7 );
            if((faces[i].width*0.6) > 70 && (faces[i].height*0.6) > 70)
            {
                ellipse( frame, center, Size( faces[i].width*0.6, faces[i].height*0.6), 0, 0, 360, Scalar( 0, 0, 255 ), 4, 8, 0 );
                face_count++;
            }
        }

        if(face_count > 0)
        {
            human_toggledetect = 1;
            imshow("Human Detection",frame);
            ///Release the frames
            frame_gray.release();
            frame.release();
            return true;
        }
        else
        {
            human_toggledetect = 0;
            imshow("Human Detection",frame);
            ///Release the frames
            frame_gray.release();
            frame.release();
            return false;
        }
    }
    else
    {
        human_toggledetect = 0;
        ///Release the frames
        imshow("Human Detection",frame);
        frame_gray.release();
        frame.release();
        return false;
    }
 }

bool doorsense(Mat imgray)
{
    float var;
    float mean_x=0.00;
    float mean_sq_error_x = 0.00;

    int *coordx = new int[2];
    int *coordy = new int[2];
    int countnum = 0;
    int togglex = 0;
    int toggley = 0;
    int a1 = 0;
    int *visit = new int[2];
    int count_black=0,count_white=0;

    for (i = 1; i < height; i++)
    {
		for (j = 1; j < width; j++)
		{
            if((int)imgray.at<uchar>(i,j)>200)
            {
                imgray.at<uchar>(i,j) = 255;
                count_white++;
            }
            else
            {
                imgray.at<uchar>(i,j) = 0;
                count_black++;
            }
        }
    }
    //cout<<"Count White "<<count_white<<endl;
    //cout<<"Count Black "<<count_black<<endl;

    for (i = 1; i < height; i++)
    {
        visit[0] = 0;
        visit[1] = 0;
		for (j = 1; j < width; j++)
		{
            if(togglex == 0)
            {
                if((int)imgray.at<uchar>(i,j-1) == 0 && (int)imgray.at<uchar>(i,j) == 255)
                {
                    //cout<<"OK1 "<<j<<endl;
                    coordx[a1] = j;
                    togglex = 1;
                    a1 = 1;
                    visit[0] = 1;
                }
            }
            else if(togglex == 1)
            {
                if((int)imgray.at<uchar>(i,j) == 255 && (int)imgray.at<uchar>(i,j+1) == 0)
                {
                    //cout<<"OK2 "<<j<<endl;
                    coordx[a1] = j;
                    togglex = 1;
                    a1 = 1;
                    visit[1] = 2;
                }
            }
        }
        if((visit[0]+visit[1]) == 3)
        {
            mean_x = mean_x + (coordx[1] - coordx[0]);
            countnum ++;
        }
        a1 = 0;
        togglex = 0;
    }
    try
    {
        mean_x = mean_x/countnum;
        //throw countnum;
    }
    catch(int)//int e)
    {
        cout<<"Divide by Zero error"<<endl;
    }

    //cout<<"mean_x "<<mean_x<<endl;

    countnum = 0;
    togglex = 0;

    for (i = 1; i < height; i++)
    {
        visit[0] = 0;
        visit[1] = 0;
		for (j = 1; j < width; j++)
		{
            if(togglex == 0)
            {
                if((int)imgray.at<uchar>(i,j-1) == 0 && (int)imgray.at<uchar>(i,j) == 255)
                {
                    //cout<<"OK3"<<endl;
                    coordx[a1] = j;
                    togglex = 1;
                    a1 = 1;
                    visit[0] = 1;
                }
            }
            else if(togglex == 1)
            {
                if((int)imgray.at<uchar>(i,j) == 255 && (int)imgray.at<uchar>(i,j+1) == 0)
                {
                    //cout<<"OK4"<<endl;
                    coordx[a1] = j;
                    togglex = 1;
                    a1 = 1;
                    visit[1] = 2;
                }
            }
        }
        if((visit[0]+visit[1]) == 3)
        {
            var = (coordx[1] - coordx[0]) - mean_x;
            if(var<0)
            {
                var = (-1)*(var);
            }
            else
            {
                var = var;
            }
            mean_sq_error_x = mean_sq_error_x + (var*var);
            countnum ++;
        }
        a1 = 0;
        togglex = 0;
    }
    try
    {
        mean_sq_error_x = mean_sq_error_x/countnum;
        mean_sq_error_x = sqrt(mean_sq_error_x);
        //throw countnum;
    }
    catch(int)//int e)
    {
        cout<<"Divide by Zero error"<<endl;
    }

    //cout<<"mean_sq_error "<<mean_sq_error_x<<endl;

    float feature=0.00,fc1,fc2,fxc1,fxc2,fxc1a,fxc2a,fxc1b,fxc2b,fxc1c,fxc2c,final1,final2,final1a,final2a,final1b,final2b,final1c,final2c;

    feature = (float)(mean_sq_error_x/mean_x);
    fc1 = 0.4;
    fc2 = 0.6;

    //cout<<"Feature "<<feature<<endl;

    fxc1a = (1/(2*3.14*279.78)) * pow(2.713,(- ( ((feature - 0.1528)*(feature - 0.1528)/(0.0249)) + (pow((count_white-5633.9),2)/(12570098)) ) ));   ///Door
    fxc2a = (1/(2*3.14*485.316)) * pow(2.713,(- ( ((feature - 0.3696)*(feature - 0.3696)/(0.0964)) + (pow((count_white-8428),2)/(9768200)) ) ));     /// No door
    final1a = (fxc1a*fc1);
    final2a = (fxc2a*fc2);


    fxc1b = (1/(2*3.14*279.78)) * pow(2.713,(- ( ((feature - 0.1028)*(feature - 0.1028)/(0.0249)) + (pow((count_white-5633.9),2)/(12570098)) ) ));   ///Door
    fxc2b = (1/(2*3.14*485.316)) * pow(2.713,(- ( ((feature - 0.3696)*(feature - 0.3696)/(0.0964)) + (pow((count_white-8428),2)/(9768200)) ) ));     /// No door
    final1b = (fxc1b*fc1);
    final2b = (fxc2b*fc2);


    fxc1c = (1/(2*3.14*279.78)) * pow(2.713,(- ( ((feature - 0.1528)*(feature - 0.1528)/(0.0249)) + (pow((count_white-3633.9),2)/(12570098)) ) ));   ///Door
    fxc2c = (1/(2*3.14*485.316)) * pow(2.713,(- ( ((feature - 0.3696)*(feature - 0.3696)/(0.0964)) + (pow((count_white-8428),2)/(9768200)) ) ));     /// No door
    final1c = (fxc1c*fc1);
    final2c = (fxc2c*fc2);

    //cout<<final1a<<","<<final2a<<endl;
    //cout<<final1b<<","<<final2b<<endl;
    //cout<<final1c<<","<<final2c<<endl;


    cout<<endl;

    delete[] coordx;
    delete[] coordy;
    delete[] visit;
    imgray.release();

    if(final1a+final1b+final1c>final2a+final2b+final2c)
    {
        door_toggledetect = 1;
        return true;

    }
    else
    {
        door_toggledetect = 0;
        return false;
    }

}


bool stairsense(Mat imgray)
{
    int count_steps = 0; ///Variable to count the number of steps
    int var=0,mean_var=0;
    int mean_steps_height=0,mode_steps_height=0;
    /** Kernel defined to validate the presence of steps or horizintal line
    *   0 0 0
    *   1 1 1
    *   1 1 1
    *   0 0 0
    **/
    int temp_var1,temp_var2;
    int prev_i = 0;
    int j;
    for (i = height-10; i >10; i--)
    {
        temp_var1 = ((int)imgray.at<uchar>(i,159)+(int)imgray.at<uchar>(i,160)+(int)imgray.at<uchar>(i,161)+(int)imgray.at<uchar>(i+1,159)+(int)imgray.at<uchar>(i+1,160)+(int)imgray.at<uchar>(i+1,161))/255;
        temp_var2 = ((int)imgray.at<uchar>(i-1,159)+(int)imgray.at<uchar>(i-1,160)+(int)imgray.at<uchar>(i-1,161)+(int)imgray.at<uchar>(i,159)+(int)imgray.at<uchar>(i,160)+(int)imgray.at<uchar>(i,161)+(int)imgray.at<uchar>(i+1,159)+(int)imgray.at<uchar>(i+1,160)+(int)imgray.at<uchar>(i+1,161)+(int)imgray.at<uchar>(i+2,159)+(int)imgray.at<uchar>(i+2,160)+(int)imgray.at<uchar>(i+2,161))/255;
        //cout<<"tempvar1 "<<temp_var1<<" tempvar2 "<<temp_var2<<endl;

        if(temp_var1  >= 3 && temp_var1<=5 && temp_var2 >= 3 && temp_var2 <= 5)
        {
            count_steps ++;
            //cout<<"coordinates "<<i<<endl;
        }
    }

    //cout<<"OK2"<<endl;
    if(count_steps>1)
    {
        int *var_diff = new int[count_steps/2];
        int *coord_steps = new int[count_steps];
        //cout<<"OK2a"<<endl;
        for (i = 10,j = 0; i < height-10; i++)
        {
            temp_var1 = ((int)imgray.at<uchar>(i,159)+(int)imgray.at<uchar>(i,160)+(int)imgray.at<uchar>(i,161)+(int)imgray.at<uchar>(i+1,159)+(int)imgray.at<uchar>(i+1,160)+(int)imgray.at<uchar>(i+1,161))/255;
            temp_var2 = ((int)imgray.at<uchar>(i-1,159)+(int)imgray.at<uchar>(i-1,160)+(int)imgray.at<uchar>(i-1,161)+(int)imgray.at<uchar>(i,159)+(int)imgray.at<uchar>(i,160)+(int)imgray.at<uchar>(i,161)+(int)imgray.at<uchar>(i+1,159)+(int)imgray.at<uchar>(i+1,160)+(int)imgray.at<uchar>(i+1,161)+(int)imgray.at<uchar>(i+2,159)+(int)imgray.at<uchar>(i+2,160)+(int)imgray.at<uchar>(i+2,161))/255;
            if(temp_var1  >= 3 && temp_var1<=5 && temp_var2 >= 3 && temp_var2 <= 5)
            {
                coord_steps[j] = i ;
                //cout<<"coordinates "<<coord_steps[j]<<endl;
                j++;
            }
        }
        //cout<<"OK2b"<<endl;
        var_diff = (int*)malloc((count_steps/2)*sizeof(int));
        if(count_steps % 2 != 0)        ///If odd number of steps count
        {
            for(i = 0,j=0; i<count_steps/2,j<count_steps - 1; i++,j+=2)
            {
                var_diff[i] = coord_steps[j+1] - coord_steps[j];
                //cout<<"odd consecutive difference "<<var_diff[i]<<endl;
            }
        }
        else                            ///If even number of steps count
        {
            for(i = 0,j=0; i<count_steps/2,j<count_steps; i++,j+=2)
            {
                var_diff[i] = coord_steps[j+1] - coord_steps[j];
                //cout<<"even consecutive difference "<<var_diff[i]<<endl;
            }
        }
        //cout<<"OK2c"<<endl;
        mean_steps_height = findmean(var_diff,count_steps/2);
        mode_steps_height = findmode(var_diff,count_steps/2);
        //cout<<"Number of Steps->"<<count_steps/2<<endl;
        //cout<<"Average Step Height->"<<mean_steps_height<<endl;
        //cout<<"mode Step Height->"<<mode_steps_height<<endl;


        float fx1=0.0,fx2=0.0;
        fx1 = (float).000796*pow((800/450),0.5)*pow(2.718,(- ( ( (count_steps/2)-6 )*( (count_steps/2)-6 )/32 ) -( ( (mean_steps_height)-24 )*( (mean_steps_height)-24 )/450 ) ));
        fx2 = (float).0053 *pow((450/128),0.5)* pow(2.718,(- ( ( (count_steps/2)-1 )*( (count_steps/2)-1 )/8 ) -( ( (mean_steps_height)-8 )*( (mean_steps_height)-8 )/128 ) ));

        //cout<<"FX1 "<<fx1<<endl;
        //cout<<"FX2 "<<fx2<<endl;

        imgray.release();

        delete[] coord_steps;
        delete[] var_diff;

        if(fx1 > fx2)
        {
            stair_toggledetect = 1;
            return true;
        }
        else if(fx2 > fx1)
        {
            stair_toggledetect = 0;
            return false;
        }
    }
    else
    {
        imgray.release();
        stair_toggledetect = 0;
        return false;
    }
}



Mat Segmentation(Mat imgray,int window_size,double t) ///Edge + surface Detection
{

    Mat variance=imgray.clone();
    int k,l;
    for (i = 0; i < (height); i++)    ///getting a white screen
    {
        for (j = 0; j < (width); j++)
        {
            if(i <= ((window_size/2)+1) )
            {
                variance.at<uchar>(i,j) = 0;
            }
            else if(j <= ((window_size/2)+1))
            {
                variance.at<uchar>(i,j) = 0;
            }
            else if(i >= (height-(window_size/2)))
            {
                variance.at<uchar>(i,j) = 0;
            }
            else if(j >= (width-(window_size/2)))
            {
                variance.at<uchar>(i,j) = 0;
            }
            else
            {
                int local_mean=0;
                for(k = i-(window_size/2);k <= i+(window_size/2);k++)
                {
                    for(l = j-(window_size/2);l <= j+(window_size/2);l++)
                    {
                            local_mean+= (int)imgray.at<uchar>(k,l);
                    }
                }

                local_mean /= (window_size*window_size);

                for(k=i-(window_size/2);k<=i+(window_size/2);k++)
                {
                    for(l=j-(window_size/2);l<=j+(window_size/2);l++)
                    {
                        variance.at<uchar>(i,j) = ((local_mean - (int)imgray.at<uchar>(k,l))*(local_mean - (int)imgray.at<uchar>(k,l)))/(window_size*window_size);
                    }
                }
            }
        }
    }
    //imshow("OrigVariance",variance);

    long double Thresh=0;
    int  variance_mean =0;
    for (i = (window_size/2)+1; i < (height-(window_size/2)+1); i++)    ///getting a white screen
    {
        for (j = (window_size/2)+1; j < (width-(window_size/2)+1); j++)
        {
            variance_mean += (int)variance.at<uchar>(i,j);
        }
    }
    //cout<<variance_mean<<endl;
    variance_mean = (variance_mean/(320-(2*(window_size/2)+1))/(240-(2*(window_size/2)+1)));
    //cout<<variance_mean<<endl;
    Thresh =  variance_mean + (t * pow(variance_mean,0.5));
    //cout<<Thresh<<endl;
    for (i = 0; i < (height); i++)    ///getting a white screen
    {
        for (j = 0; j < (width); j++)
        {
            if(i <= ((window_size/2)+1) )
            {
                variance.at<uchar>(i,j) = 0;
            }
            else if(j <= ((window_size/2)+1))
            {
                variance.at<uchar>(i,j) = 0;
            }
            else if(i >= (height-(window_size/2)))
            {
                variance.at<uchar>(i,j) = 0;
            }
            else if(j >= (width-(window_size/2)))
            {
                variance.at<uchar>(i,j) = 0;
            }
            else
            {
                if((int)variance.at<uchar>(i,j) > 8)
                    variance.at<uchar>(i,j) = 255;
                else
                    variance.at<uchar>(i,j) = 0;
            }
        }
    }

    long int ct = 0;
    for (i = 0; i < (height); i++)    ///getting a white screen
    {
        for (j = 0; j < (width); j++)
        {
            if( (int)variance.at<uchar>(i,j) == 255 )
                ct++;
        }
    }

    //cout<<"pixel count->"<<ct<<endl;

    return variance;
}

bool ground_segregate(Mat ground)
{
    /**********************************************************Section 2***************************************************************************/
    ///Logically deducing the ground safe region
    int check;

    pixel = new int[318];  ///Array for storing the number of pixels
    Mat exposegnd = ground.clone();
    Mat areaexpo(240,320, CV_8UC3, Scalar(0,0,0));
    Mat safereg(240,320, CV_8UC3, Scalar(0,0,0));

    for(j = 0; j < 318; j++)    ///Setting the pixel values to 0 at first
    {
        pixel [j] = 0;
    }
    for (j = 1; j < width; j++)
	{
        i = height-1;
		while(i > height/2)
		{
            ///condition for checking a window of 9 pixel travelling from the bottom of the image to half the height and set the portion as open ground in red colour
            check = ((int)ground.at<uchar>(i,j)+(int)ground.at<uchar>(i-1,j-1)+(int)ground.at<uchar>(i,j-1)+(int)ground.at<uchar>(i+1,j-1)+(int)ground.at<uchar>(i-1,j)+(int)ground.at<uchar>(i+1,j)+(int)ground.at<uchar>(i-1,j+1)+(int)ground.at<uchar>(i,j+1)+(int)ground.at<uchar>(i+1,j+1))/255;
            if(check <= 5)
            {
                i--;
                areaexpo.at<Vec3b>(i,j)[0] = 0;
                areaexpo.at<Vec3b>(i,j)[1] = 255;
                areaexpo.at<Vec3b>(i,j)[2] = 0;
            }
            else
            {
                break;
            }
        }
        pixel[j-1] =239-i;  ///When ever the discontinuity is attained stop travelling and set the number of pixel per column inpixel array and break
    }



    ///Among all the safe boundaries find the most spacious box to travel through
    toggle = 1; /// toggle 1 is equal to the region of unsafe and toggle 0 indicate safe zone
    for(j = 1; j < width; j++)
    {
        if(pixel[j-1]>100)
        {
            if(toggle == 1)
            {
                starting = j;
                toggle = 0;
            }
        }
        if(pixel[j-1]<100)
        {
            if(toggle == 0)
            {
                ending = j;
                toggle = 1;
                difference = ending - starting;
                if(difference >=65)
                {
                    break;
                }

            }
        }
        if(j == width-1)
        {
            ending = j;
            toggle = 1;
            difference = ending - starting;
            if(difference >=65)
            {
                break;
            }
        }
    }

    int num;
    long int count_white = 0, count_black = 0, red_pixel = 0;
    for(j = starting; j <= ending; j++)
    {
        i = height - 1;
        num = pixel[j-1];
        while(num)
        {
            safereg.at<Vec3b>(i,j)[0] = 0;
            safereg.at<Vec3b>(i,j)[1] = 0;
            safereg.at<Vec3b>(i,j)[2] = 255;
            i--;
            num--;
            red_pixel ++;
        }
    }

    for (i = 1; i < height; i++)
    {
        for (j = 1; j < width; j++)
        {
            if((int)ground.at<uchar>(i,j) >= 250)
            {
                count_white++;
            }
            else
            {
                count_black++;
            }
        }
    }

    imshow("Area Exposed",areaexpo);
    imshow("Safe Region",safereg);

    safereg.release();
    areaexpo.release();
    exposegnd.release();

    ///cout<<"Red Pixel->"<<red_pixel<<",White Pixel->"<<count_white<<",Black Pixel->"<<count_black<<endl;
    if(red_pixel > 19000 && count_white < 1600)
    {
        //cout<<((starting+ending)/2)<<endl;
        return true;
    }
    else
    {
        return false;
    }


}

Mat DoorHough(Mat ImTrans)
{
    Mat cdst(240,320,CV_8UC1,Scalar(0));
    vector<Vec2f> lines;
    HoughLines(ImTrans, lines, 2, CV_PI*3/180, 150, 0, 0 );

    if(lines.size() < 100)
    {
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];
            if(theta<(CV_PI/18) && theta>-(CV_PI/18))
            {
                Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                line( cdst, pt1, pt2, Scalar(255), 1, CV_AA);
            }
        }
    }

    return cdst;
}

Mat StairHough(Mat ImTrans)
{
    Mat cdst(240,320,CV_8UC1,Scalar(0));
    vector<Vec2f> lines;
    HoughLines(ImTrans, lines, 3, CV_PI*2/180, 130, 0, 0 );

    if(lines.size() < 100)
    {
        for( size_t i = 0; i < lines.size(); i++ )
        {
            float rho = lines[i][0], theta = lines[i][1];

            if ((theta>1.48) && theta<(1.65))
            {
                Point pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                line( cdst, pt1, pt2, Scalar(255), 1, CV_AA);
            }
        }
    }

    return cdst;
}

