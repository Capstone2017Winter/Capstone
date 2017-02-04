package StereoMatch;

import java.awt.image.BufferedImage;
import java.io.*;
import java.net.*;
import javax.imageio.*;

public class server {
	private static URL target;
	private static BufferedImage image;
	private static File output;
	private static URLConnection sendinfo;
	private static BufferedReader key;
	private static BufferedReader in;

	public static void main(String[] args) {
		key = new BufferedReader(new InputStreamReader(System.in));
		try {
			// StereoMatch.Match("image/leftTimo.jpg", "image/rightTimo.jpg");
			String response = "";
			int count = 0;

			System.out.println("Server Started, "
					+ "please press enter to initialize a thread");
			boolean cont = true;
			while (cont) {
				if (key.readLine().equals("q")) {
					System.out.println("Closing Server, Thank you");
					cont = false;
				} else {
					System.out.println("Starting Sequence " + (++count));
					System.out.println("Enter action (test, "
							+ "demoTimo, demoCar, demoLeo, capture)");
					String value = key.readLine();
					if (value.equals("capture")) {
						System.out.println("Starting... "
								+ "enter \"c\" to see status "
								+ "and initiate download");

						target = new URL("http://192.168.1.200/CAPTURE");
						sendinfo = (HttpURLConnection) target.openConnection();
						sendinfo.setDoOutput(true);
						sendinfo.setDoInput(true);
						in = new BufferedReader(new InputStreamReader(
								sendinfo.getInputStream()));
						response = in.readLine();
						System.out.println(response);
						in.close();
						while (true) {
							if (value.equals("c")) {
								target = new URL(
										"http://192.168.1.200/IMAGES_READY");
								sendinfo = (HttpURLConnection) target
										.openConnection();
								sendinfo.setDoOutput(true);
								sendinfo.setDoInput(true);
								in = new BufferedReader(new InputStreamReader(
										sendinfo.getInputStream()));
								response = in.readLine();
								System.out.println(response);
								in.close();
							}
							if (response.equals("READY")) {
								break;
							} else {
								value = key.readLine();
							}
						}
						try {
						System.out.println("Reading left image");
						target = new URL("http://192.168.1.200/left.jpg");;
						image = ImageIO.read(target);
						output = new File("image/left.jpg");
						if (image != null) {
							ImageIO.write(image, "JPG", output);
							System.out.println("Left Image Downloaded");
							System.out.println("Reading right image");
							target = new URL("http://192.168.1.200/right.jpg");
							image = ImageIO.read(target);
							output = new File("image/right.jpg");
							if (image != null) {
								ImageIO.write(image, "JPG", output);
								System.out.println("Right Image Downloaded");
								System.out.println("Images Read, analyzing...");
								StereoMatch.Match("image/left.jpg",
										"image/right.jpg");
							} else {
								System.out
										.println("Couldn't download Right Image");
							}
						} else {
							System.out.println("Couldn't download Left Image");
						}
						} catch (Exception e) {
							System.out.println("One of the images was unreadable, please try again");
						}
					}
					if (value.equals("test")) {
						System.out.println("Starting test image download...");
						target = new URL(
								"http://192.168.1.200/just-a-board.jpg");
						image = ImageIO.read(target);
						System.out.println("Read in image, analyzing...");
						output = new File("image/just-a-board.jpg");
						ImageIO.write(image, "JPG", output);
					} else if (value.equals("demoTimo")) {
						System.out.println("Starting test images download...");
						target = new URL("http://192.168.1.200/leftTimo.jpg");
						image = ImageIO.read(target);
						output = new File("image/leftTimo.jpg");
						System.out.println("Finished left image");
						ImageIO.write(image, "JPG", output);
						target = new URL("http://192.168.1.200/rightTimo.jpg");
						image = ImageIO.read(target);
						output = new File("image/rightTimo.jpg");
						ImageIO.write(image, "JPG", output);
						System.out.println("Finished right image");
						System.out.println("Read in image, analyzing...");
						StereoMatch.Match("image/leftTimo.jpg",
								"image/rightTimo.jpg");
					} else if (value.equals("demoCar")) {
						System.out.println("Starting test image download...");
						target = new URL("http://192.168.1.200/leftCar.jpg");
						image = ImageIO.read(target);
						output = new File("image/leftCar.jpg");
						ImageIO.write(image, "JPG", output);
						target = new URL("http://192.168.1.200/rightCar.jpg");
						image = ImageIO.read(target);
						output = new File("image/rightCar.jpg");
						ImageIO.write(image, "JPG", output);
						System.out.println("Read in image, analyzing...");
						StereoMatch.Match("image/leftCar.jpg",
								"image/rightCar.jpg");
					} else if (value.equals("demoLeo")) {
						System.out.println("Starting test image download...");
						target = new URL("http://192.168.1.200/leftLeo.jpg");
						image = ImageIO.read(target);
						output = new File("image/leftLeo.jpg");
						ImageIO.write(image, "JPG", output);
						target = new URL("http://192.168.1.200/rightLeo.jpg");
						image = ImageIO.read(target);
						output = new File("image/rightLeo.jpg");
						ImageIO.write(image, "JPG", output);
						System.out.println("Read in image, analyzing...");
						StereoMatch.Match("image/leftLeo.jpg",
								"image/rightLeo.jpg");
					}
					System.out.println("Done Thread!");
				}
			}
			System.exit(0);
		} catch (Exception io) {
			io.printStackTrace();
		}
	}

}
