package com.example.andsifttest1;



import java.util.List;

import android.graphics.Bitmap;

/**
 * Every Algorithm which creates robust features from Images have to implement this interface. Then it could be easly integrated into the
 * ImageAnalyzerImplementation.
 * @author tschinke
 */
public interface ImageFeatureExtractor {

    /**
     * Analyzes the given image and extracts a set of features describing the image
     * @param img (not null)
     * @return a List of identified feautures
     */
    public List<ImageFeature> getFeatures(Bitmap img);
}

