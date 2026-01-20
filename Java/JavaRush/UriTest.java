import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.file.Files;
import java.nio.file.Path;

public class UriTest {
    public static void main(String[] args) throws InterruptedException, IOException {
        URI uri = URI.create("https://raw.githubusercontent.com/YerdosNar/YerdosNar/master/assets/arch.png");
        HttpClient client = HttpClient.newHttpClient();
        HttpRequest req = HttpRequest.newBuilder(uri).build();

        HttpResponse<byte[]> res = client.send(req, HttpResponse.BodyHandlers.ofByteArray());
        if (res.statusCode() == 200) {
            Files.write(Path.of("test.sh"), res.body());
        }
        else {
            System.out.println("ERROR: code " + res.statusCode());
        }
    }
}
