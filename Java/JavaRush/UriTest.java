import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Duration;

public class UriTest {
    public static void main(String[] args) throws InterruptedException, IOException {
        URI uri = URI.create("https://raw.githubusercontent.com/YerdosNar/YerdosNar/master/assets/arch.png");
        HttpClient client = HttpClient.newHttpClient();
        HttpRequest req = HttpRequest.newBuilder(uri)
                                    .timeout(Duration.ofSeconds(5)) // Do not wait indefinitely
                                    .build();

        HttpResponse<byte[]> res = client.send(req, HttpResponse.BodyHandlers.ofByteArray());
        if (res.statusCode() == 200) {
            String type = res.headers().firstValue("Content-Type").orElse("unknown");
            System.out.println("Type: " + type);
            String length = res.headers().firstValue("Content-Length").orElse("unknown size");
            System.out.println("Size: " + length + "bytes");
            Files.write(Path.of("test_file"), res.body());
        }
        else {
            System.out.println("ERROR: code " + res.statusCode());
        }
    }
}
