import java.io.IOException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;

public class UrlTest {
    public static void main(String[] args) throws IOException {
        URL url = new URL("https://raw.githubusercontent.com/YerdosNar/3x-ui-auto/master/install.sh");
        Files.copy(url.openStream(), Path.of("test.sh"));
    }
}
