/*
 * Copyright (c) 2011-2013, Peter Abeles. All Rights Reserved.
 *
 * This file is part of BoofCV (http://boofcv.org).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//package boofcv.examples;

package StereoMatch;

import boofcv.abst.feature.disparity.StereoDisparity;
import boofcv.alg.distort.ImageDistort;
import boofcv.alg.geo.PerspectiveOps;
import boofcv.alg.geo.RectifyImageOps;
import boofcv.alg.geo.rectify.RectifyCalibrated;
import boofcv.core.image.ConvertBufferedImage;
import boofcv.factory.feature.disparity.DisparityAlgorithms;
import boofcv.factory.feature.disparity.FactoryStereoDisparity;
import boofcv.gui.image.ShowImages;
import boofcv.gui.image.VisualizeImageData;
import boofcv.io.image.UtilImageIO;
//import boofcv.misc.BoofMiscOps;
import boofcv.struct.calib.IntrinsicParameters;
import boofcv.struct.calib.StereoParameters;
import boofcv.struct.image.ImageUInt8;
import georegression.struct.point.Vector3D_F64;
import georegression.struct.se.Se3_F64;
import org.ejml.data.DenseMatrix64F;

import java.awt.image.BufferedImage;

/**
 * The disparity between two stereo images is used to estimate the range of
 * objects inside the camera's view. Disparity is the difference in position
 * between the viewed location of a point in the left and right stereo images.
 * Because input images are rectified, corresponding points can be found by only
 * searching along image rows.
 * 
 * Values in the disparity image specify how different the two images are. A
 * value of X indicates that the corresponding point in the right image from the
 * left is at "x' = x - X - minDisparity", where x' and x are the locations in
 * the right and left images respectively. An invalid value with no
 * correspondence is set to a value more than (max - min) disparity.
 * 
 * @author Peter Abeles
 */
public class StereoMatch {

	/**
	 * Computes the dense disparity between between two stereo images. The input
	 * images must be rectified with lens distortion removed to work! Floating
	 * point images are also supported.
	 * 
	 * @param rectLeft
	 *            Rectified left camera image
	 * @param rectRight
	 *            Rectified right camera image
	 * @param minDisparity
	 *            Minimum disparity that is considered
	 * @param maxDisparity
	 *            Maximum disparity that is considered
	 * @return Disparity image
	 */
	public static ImageUInt8 denseDisparity(ImageUInt8 rectLeft,
			ImageUInt8 rectRight, int minDisparity, int maxDisparity) {
		// A slower but more accuracy algorithm is selected
		// All of these parameters should be turned
		StereoDisparity<ImageUInt8, ImageUInt8> disparityAlg = FactoryStereoDisparity
				.regionWta(DisparityAlgorithms.RECT_FIVE, minDisparity,
						maxDisparity, 3, 3, 20, 1, 0.2, ImageUInt8.class);

		// process and return the results
		disparityAlg.process(rectLeft, rectRight);

		return disparityAlg.getDisparity();
	}

	/**
	 * Rectified the input images using known calibration.
	 */
	public static void rectify(ImageUInt8 origLeft, ImageUInt8 origRight,
			StereoParameters param, ImageUInt8 rectLeft, ImageUInt8 rectRight) {
		// Compute rectification
		RectifyCalibrated rectifyAlg = RectifyImageOps.createCalibrated();
		Se3_F64 leftToRight = param.getRightToLeft().invert(null);

		// original camera calibration matrices
		DenseMatrix64F K1 = PerspectiveOps.calibrationMatrix(param.getLeft(),
				null);
		DenseMatrix64F K2 = PerspectiveOps.calibrationMatrix(param.getRight(),
				null);

		rectifyAlg.process(K1, new Se3_F64(), K2, leftToRight);

		// rectification matrix for each image
		DenseMatrix64F rect1 = rectifyAlg.getRect1();
		DenseMatrix64F rect2 = rectifyAlg.getRect2();
		// New calibration matrix,
		DenseMatrix64F rectK = rectifyAlg.getCalibrationMatrix();

		// Adjust the rectification to make the view area more useful
		RectifyImageOps.fullViewLeft(param.left, rect1, rect2, rectK);

		// undistorted and rectify images
		ImageDistort<ImageUInt8> imageDistortLeft = RectifyImageOps
				.rectifyImage(param.getLeft(), rect1, ImageUInt8.class);
		ImageDistort<ImageUInt8> imageDistortRight = RectifyImageOps
				.rectifyImage(param.getRight(), rect2, ImageUInt8.class);

		imageDistortLeft.apply(origLeft, rectLeft);
		imageDistortRight.apply(origRight, rectRight);
	}

	// public static void main( String args[] ) {
	public static void Match(String leftImage, String rightImage) {
		/* Example Parameters from different camera
		 * IntrinsicParameters left = new IntrinsicParameters(
		 * 524.6141554796689, 526.9202267407102, 0.0, 312.4508572809385,
		 * 247.40163214599613, 640, 480, false, new
		 * double[]{-0.3625365821238732, 0.17532754393079863} );
		 * IntrinsicParameters right = new IntrinsicParameters(
		 * 526.437309515818, 528.6524795626539, 0.0, 318.40405668073225,
		 * 245.93504188705572, 640, 480, false, new
		 * double[]{-0.35299970494414057, 0.16297446727663975} ); Se3_F64
		 * modifier = new Se3_F64(new DenseMatrix64F( 3, 3, true, new double[] {
		 * 0.9999694757041413, -0.002167658916924345, 0.007506591423858594,
		 * 0.002144760822245101, 0.9999930270271246, 0.0030571061711613945,
		 * -0.007513165844052039, -0.00304091301195358, 0.999967152054033 } ),
		 * new Vector3D_F64( 120.00384197947756, 0.8688852783418195,
		 * -0.9149583203134171 ));
		 */
		/* Calibrated Parameters */
		IntrinsicParameters left = new IntrinsicParameters(
				696.8901211230207, 697.2347476614341,
				0.0,
				313.85442149707364,
				278.15765458998214, 640, 480, false, new double[] { -0.40776996222024614, 0.14705435052735114 });
		IntrinsicParameters right = new IntrinsicParameters(
				685.6795873286699, 685.8595268541417, 
				0.0,
				337.2197758262941,
				277.2615141239774, 640, 480, false, new double[] { -0.4191228450751954, 0.16290544291159584 });
		Se3_F64 modifier = new Se3_F64(new DenseMatrix64F(3, 3, true,
				new double[] { 
						0.9999715828342303,
						-0.0074697025281216835,
						-0.0010183654282296672,
						0.007457751404515943,
						0.999908640040822,
						-0.011273575994433552,
						0.001102482649512345,
						0.01126566091515304,
						0.9999359326557644
					}), new Vector3D_F64(
				131.13271355273423, -0.38649758893056685, 1.3114297250251639));
		StereoParameters param = new StereoParameters(left, right, modifier);
		System.out.println(param.getBaseline());

		// load and convert images into a BoofCV format
		BufferedImage origLeft = UtilImageIO.loadImage(leftImage);
		BufferedImage origRight = UtilImageIO.loadImage(rightImage);

		ImageUInt8 distLeft = ConvertBufferedImage.convertFrom(origLeft,
				(ImageUInt8) null);
		ImageUInt8 distRight = ConvertBufferedImage.convertFrom(origRight,
				(ImageUInt8) null);

		// rectify images
		ImageUInt8 rectLeft = new ImageUInt8(distLeft.width, distLeft.height);
		ImageUInt8 rectRight = new ImageUInt8(distRight.width, distRight.height);

		rectify(distLeft, distRight, param, rectLeft, rectRight);

		// compute disparity
		ImageUInt8 disparity = denseDisparity(rectLeft, rectRight, 1, 150);

		// show results
		BufferedImage visualized = VisualizeImageData.disparity(disparity,
				null, 1, 150, 0);
		ShowImages.showWindow(rectLeft, "Left");
		ShowImages.showWindow(rectRight, "right");
		ShowImages.showWindow(visualized, "Disparity");
	}
}
