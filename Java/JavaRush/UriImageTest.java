import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;

import javax.imageio.ImageIO;

public class UriImageTest {
    public static void main(String[] args) throws InterruptedException, IOException {
        InputStream in = new URL("https://raw.githubusercontent.com/YerdosNar/YerdosNar/master/assets/arch.png").openStream();
        BufferedImage img = ImageIO.read(in);
        System.out.println("Width: " + img.getWidth() + "\nHeight: " + img.getHeight());
    }
}
