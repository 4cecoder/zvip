#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <libxml2/libxml/HTMLparser.h>
#include <libxml2/libxml/xpath.h>

#define BASE_URL "https://vipstream.tv"
#define PROVIDER "Vidcloud"

char *search_movie(const char *query) {
  CURL *curl;
  CURLcode res;
  char *response = NULL;

  curl = curl_easy_init();
  if (curl) {
    char url[strlen(BASE_URL) + strlen(query) + 9];
    sprintf(url, "%s/search/%s", BASE_URL, query);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      free(response);
      response = NULL;
    }
  }

  return response;
}

char *get_movie_page(const char *movie_id) {
  CURL *curl;
  CURLcode res;
  char *response = NULL;

  curl = curl_easy_init();
  if (curl) {
    char url[strlen(BASE_URL) + 28];
    sprintf(url, "%s/ajax/movie/episodes/%s", BASE_URL, movie_id);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
      free(response);
      response = NULL;
    }
  }

  return response;
}

char *get_embed_link(const char *episode_id) {
  CURL *curl;
  CURLcode res;
  char *response = NULL;
  curl = curl_easy_init();
  if (curl) {
    char url[strlen("https://flixhq.to/ajax/sources/") + strlen(episode_id) + 1];
    sprintf(url,"https://vipstream.tv/ajax/sources/%s", episode_id);

  // retrieve key from key.txt
  char key[512];
  snprintf(key, sizeof key, "https://github.com/enimax-anime/key/blob/e%s/key.txt", embed_type);
  FILE *keyfile = fopen(key, "r");
  if (!keyfile) {
    perror("Error opening key file");
    return 1;
  }
  if (fgets(key, sizeof key, keyfile) == NULL) {
    perror("Error reading key file");
    return 1;
  }
  fclose(keyfile);

  // retrieve encrypted video link from API
  char command[strlen("curl -s ") + strlen(provider_link) + strlen("/ajax/embed-") + strlen(embed_type) + strlen("/getSources?id=") + strlen(source_id) + strlen(" -H X-Requested-With: XMLHttpRequest") + 1];
  sprintf(command,
          "curl -s %s/ajax/embed-%s/getSources?id=%s -H X-Requested-With: XMLHttpRequest",
          provider_link, embed_type, source_id);
  FILE *enc_video = popen(command, "r");
  if (!enc_video) {
    perror("Error retrieving encrypted video link");
    return 1;
  }
  char enc_link[2048];
  if (fgets(enc_link, sizeof enc_link, enc_video) == NULL) {
    perror("Error reading encrypted video link");
    return 1;
  }
  pclose(enc_video);

  // decode base64 encoded video link
  size_t decoded_size = 0;
  unsigned char *decoded_link = base64_decode(enc_link, strlen(enc_link), &decoded_size);
  if (!decoded_link) {
    perror("Error decoding base64 encoded video link");
    return 1;
  }

  // decrypt video link
  unsigned char *video_link = aes_decrypt(decoded_link, decoded_size, key, strlen(key));
  if (!video_link) {
    perror("Error decrypting video link");
    return 1;
  }

  printf("Decrypted video link: %s\n", video_link);
  free(decoded_link);
  free(video_link);
  return 0;

  }
  int main() {
    char episode_id[100];
    printf("Enter episode ID: ");
    scanf("%s", episode_id);

    char url[strlen("https://vipstream.tv/ajax/sources/") + strlen(episode_id) + 1];
    sprintf(url, "https://vipstream.tv/ajax/sources/%s", episode_id);

    // Perform HTTP GET request to the URL
    // Store the response in a string
    // ...

    // Parse the response string into a JSON object
    // ...

    // Extract the necessary information from the JSON object
    // ...

    // Perform any desired actions with the extracted information
    // ...

    return 0;
  }
