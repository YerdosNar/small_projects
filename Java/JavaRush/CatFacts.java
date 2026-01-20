import java.net.http.*;
import java.io.IOException;
import java.net.URI;

public class CatFacts {
    public static void main(String[] args) throws InterruptedException, IOException {
        String url = "https://catfact.ninja/fact";

        HttpClient client = HttpClient.newHttpClient();
        HttpRequest req = HttpRequest.newBuilder(URI.create(url)).GET().build();
        HttpResponse<String> resp = client.send(req, HttpResponse.BodyHandlers.ofString());

        System.out.println(resp.body());
    }
}
