import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;

public class Weather {
    public static void main(String[] args) throws InterruptedException, IOException {
        String url = "https://api.open-meteo.com/v1/forecast?latitude=50.45&longitude=30.52&current_weather=true";
        URI uri = URI.create(url);
        HttpClient client = HttpClient.newHttpClient();
        HttpRequest req = HttpRequest.newBuilder(uri).GET().build();
        HttpResponse<String> res = client.send(req, HttpResponse.BodyHandlers.ofString());

        System.out.println("HTTP status: " + res.statusCode());
        System.out.println("Response: ");
        System.out.println(res.body());
    }
}
