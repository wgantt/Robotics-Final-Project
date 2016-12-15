/****************************************************************************
 * Inputs:      the spot whose brightness is to be checked
 * Outputs:     the median brightness of the inner region of the spot
 * Description: the purpose of this function is to determine whether there
 *              is any significant difference between the brightness (y 
 *              values) of true positive and false positive black spots.
 *              Given a spot, the function simply returns the median y value
 *              of all the pixels in its inner region.
 ***************************************************************************/
float BallDetector::getMedianBrightness(Spot spot) {

    // convert to raw coordinates
    int leftX = spot.ix() + width / 2 - spot.innerDiam / 4;
    int rightX = spot.ix() + width / 2 + spot.innerDiam / 4;
    int bottomY = -spot.iy() + height / 2 + spot.innerDiam / 4;
    int topY = -spot.iy() + height / 2 - spot.innerDiam / 4;

    int numPixels = 0;        // the number of pixels checked
    std::vector<int> yValues; // a vector for all the y values

    // iterate through all the pixels in the inner region
    for (int y=topY; y<=bottomY; y++) {
      for (int x=leftX; x<=rightX; x++, numPixels++) {

        // add the y value of the current pixel to the vector
        // of all y values
        yValues.push_back(*(yImage.pixelAddr(x,y)) / 4); 
      }
    }
    // sort the y values
    std::sort(yValues.begin(), yValues.end());

    return yValues[yValues.size() / 2];

} // end getMedianBrightness


/********************************************************************
 * Inputs:      the white spot to check
 *
 * Outputs:     the median brightness values of the top and bottom
 *              hemispheres, as a pair of doubles (these could just
 *              as well be integers)
 *
 * Description: compares the median brightness of pixels in the
 *              top half of the ball to that of pixels in the bottom
 *              half. 
 ********************************************************************/
std::pair<double,double> BallDetector::topOfBallBrighterThanBottomMedian(Spot spot) {

    // convert to raw coordinates
    int leftX = spot.ix() + width / 2 - spot.innerDiam / 4;
    int rightX = spot.ix() + width / 2 + spot.innerDiam / 4;
    int midX = spot.ix() + width / 2;

    printf("X: [%d, %d, %d]\n", leftX, rightX, midX);

    int bottomY = -spot.iy() + height / 2 + spot.innerDiam / 4;
    int topY = -spot.iy() + height / 2 - spot.innerDiam / 4;
    int midY = -spot.iy() + height / 2;

    printf("Y: [%d, %d, %d]\n", bottomY, topY, midY);

    // counters for the number of top and bottom pixels checked
    int topPixels = 0;
    int bottomPixels = 0;

    // vectors for the brightness values
    std::vector<int> topYvalues;
    std::vector<int> bottomYvalues;

    // put the y values of the pixels in the top half of the
    // ball into the topYvalues vector
    for (int y=topY; y<midY; y++) {
      for (int x=leftX; x<=rightX; x++, topPixels++) {

        if (debugBall) // show the region checked
          debugDraw.drawDot(x,y,WHITE);
        topYvalues.push_back(*(yImage.pixelAddr(x,y)));
      }
    }

    // put the y values of the pixels in the bottom half of the
    // ball into the bottomYvalues vector
    for (int y=midY + 1; y<=bottomY; y++) {
      for (int x=leftX; x<=rightX; x++, bottomPixels++) {

        if (debugBall) // show the region checked
          debugDraw.drawDot(x,y,BLACK);
        bottomYvalues.push_back(*(yImage.pixelAddr(x,y))); 
      }
    }

    // sort both vectors
    std::sort(topYvalues.begin(), topYvalues.end());
    std::sort(bottomYvalues.begin(), bottomYvalues.end());

    // get the median brightness of each (have to divide by 4 because
    // of a weird property of pixels in the y image)
    float topMedian = topYvalues[topYvalues.size() / 2] / 4;
    float bottomMedian = bottomYvalues[bottomYvalues.size() / 2] / 4;
    
    // return the medians in pair form
    std::pair<double,double> medianBrightnesses = 
	    std::make_pair(topMedian, bottomMedian);

    printf("Top of ball has median brightness %f\n", topMedian);
    printf("Bottom of ball has median brightness %f\n", bottomMedian);

    return medianBrightnesses;

} // end topOfBallBrighterThanBottomMedian


/********************************************************************
 * Inputs:      the white spot to check
 *
 * Outputs:     the median brightness values of rectangular regions
 *              directly above and below the ball.
 *
 * Description: This function uses the assumption that there will
 *              most likely be a shawdow below a ball, and thus that
 *              the area below the ball will be darker than the area
 *              above the ball. This function compares the median
 *              brightness of the two areas.
 ********************************************************************/

std::pair<int,int> BallDetector::aboveBallBrighterThanBelowBall(Spot spot) {

    int leftX = spot.ix() + width / 2 - spot.outerDiam / 4;
    int rightX = spot.ix() + width / 2 + spot.outerDiam / 4;

    //Top Rect
    int topRectBottomY = -spot.iy() + height / 2 - spot.outerDiam / 4;
    int topRectTopY = -spot.iy() + height / 2 - 
	    (spot.outerDiam + spot.innerDiam) / 4;

    //Bottom Rect
    int bottomRectTopY = -spot.iy() + height / 2 + spot.outerDiam / 4;
    int bottomRectBottomY = -spot.iy() + height / 2 + 
	    (spot.outerDiam + spot.innerDiam) / 4;

    // vectors for the y values of the above and below ball regions
    std::vector<int> topYValues;
    std::vector<int> bottomYValues;

    // Check Top Rect
    for (int x = leftX; x <= rightX; x++) {
        for (int y = topRectTopY; y <= topRectBottomY; y++) {
            if (debugBall) {
                debugDraw.drawDot(x,y,RED);
            }
            topYValues.push_back(*(yImage.pixelAddr(x,y)));
        }
    }

    // Check Bottom Rect
    for (int x = leftX; x <= rightX; x++) {
        for (int y = bottomRectTopY; y <= bottomRectBottomY; y++) {
            if (debugBall) {
                debugDraw.drawDot(x,y,BLUE);
            }
            bottomYValues.push_back(*(yImage.pixelAddr(x,y)));
        }
    }

    // sort both vectors
    std::sort(topYValues.begin(), topYValues.end());
    std::sort(bottomYValues.begin(), bottomYValues.end());

    // get the median y values for both regions
    int topMedian = topYValues[topYValues.size() / 2] / 4;
    int bottomMedian = bottomYValues[bottomYValues.size() / 2] / 4;

    // return an integer pair of the medians
    std::pair<int,int> medianBrightnesses = 
	    std::make_pair(topMedian, bottomMedian);

    printf("Area above ball has median brightness %d\n", topMedian);
    printf("Area below ball has median brightness %d\n", bottomMedian);

    return medianBrightnesses;

} // end aboveBallBrighterThanBelowBall
