#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

void generate_json(const char *directory, const char *output_file) {
    FILE *json_file = fopen(output_file, "w");
    if (!json_file) {
        perror("Failed to open output file");
        return;
    }

    fprintf(json_file, "[\n");

    DIR *dir = opendir(directory);
    if (!dir) {
        perror("Failed to open directory");
        fclose(json_file);
        return;
    }

    struct dirent *entry;
    int first = 1;

    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (entry->d_name[0] == '.')
            continue;

        // Check for image files (basic check for .jpg, .png, etc.)
        char *ext = strrchr(entry->d_name, '.');
        if (ext && (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".png") == 0 || strcmp(ext, ".gif") == 0)) {
            // Build .url filename
            char url_file_path[512];
            snprintf(url_file_path, sizeof(url_file_path), "%s/%s.url", directory, entry->d_name);

            // Open .url file to read the URL
            FILE *url_file = fopen(url_file_path, "r");
            if (!url_file) {
                continue; // Skip if no .url file exists
            }

            char url[256];
            if (fgets(url, sizeof(url), url_file)) {
                url[strcspn(url, "\n")] = 0; // Remove newline character
            }
            fclose(url_file);

            if (!first) {
                fprintf(json_file, ",\n");
            }
            first = 0;

            // Write JSON entry
            fprintf(json_file, "  {\n    \"name\": \"%s\",\n    \"url\": \"%s\"\n  }",
                    entry->d_name, url);
        }
    }

    fprintf(json_file, "\n]\n");
    closedir(dir);
    fclose(json_file);
}

int main() {
    const char *directory = "./src"; // Directory containing images and .url files
    const char *output_file = "./data.json";

    generate_json(directory, output_file);

    printf("JSON file generated: %s\n", output_file);
    return 0;
}
