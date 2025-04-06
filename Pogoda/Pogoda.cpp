// Pogoda.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <locale.h>
#include <curl/curl.h>
#include <json/json.h>
#include <string>


// Funkcja przetwarzająca dane z CURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t bytes = size * nmemb;
	
	// (string*)userp rzutowanie void* na string
	// (char*)contents rzutowanie void* na char*
	// append(..., size * nmemb) dodaj dokładnie tyle bajtów ile danych zostanie odebranych
	((std::string*)userp)->append((char*)contents, bytes);
	return bytes;
}

// Funkcja wykonująca zapytanie CURL
bool performCurlRequest(const std::string& url, std::string& response) {
	CURL* curl = curl_easy_init(); // Inicjalizacja CURL

	if (!curl) {
		std::cerr << "Failed to initialize CURL" << std::endl;
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // Ustawienie url do którego CURL ma wysłać zapytanie, konwersja string na char*
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // wysłanie pobranych danych do funkcji
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response); // przekazanie response do funkcji WriteCallback jako userp

	CURLcode res = curl_easy_perform(curl); //	Wysyła zapytanie o status
	curl_easy_cleanup(curl); // Zwalnianie zasobów CURL

	if (res != CURLE_OK) {
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
		return false;
	}
	return true;
}


// Funkcja analizująca dane zwrotne JSON
bool parseJsonResponse(const std::string& jsonResponse, Json::Value& parsedRoot) {
	Json::Reader reader; // Tworzenie obiektu JSON, który parsuje tekstu JSON na Json::Value

	bool parsingSuccessful = reader.parse(jsonResponse, parsedRoot); // Parsowanie tekstu

	if (!parsingSuccessful) {
		std::cerr << "Failed to parse JSON: " << std::endl;
		return false;
	}

	return true;
}

int main() {
	setlocale(LC_CTYPE, "Polish"); // Poprawne wyświetlanie polskich zanaków

	//std::string api_url = "google.com";
	std::string api_url = "https://api.gios.gov.pl/pjp-api/rest/station/findAll";
	std::string response;

	curl_global_init(CURL_GLOBAL_DEFAULT); // Globalna inicjalizacja CURL

	if(performCurlRequest(api_url, response)) {
		Json::Value root; // Inicjowanie zmiennej biblioteki JSON
		if (parseJsonResponse(response, root)) {
			for (Json::Value::const_iterator outer = root.begin(); outer != root.end(); outer++)
			{
				std::cout << "Station: " << root[outer.index()]["stationName"] << std::endl;
			}
		}
	}

	curl_global_cleanup(); // Globalne zwalnianie zasobów CURL
	return 0;
}