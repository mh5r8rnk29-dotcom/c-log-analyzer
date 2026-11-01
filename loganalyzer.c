#include <stdio.h>
#include <string.h>

#define LINE_SIZE 8192
#define IP_SIZE 46
#define METHOD_SIZE 16
#define PATH_SIZE 2048
#define PROTOCOL_SIZE 16

int main(int argc, char *argv[])
{
    FILE *fp;
    char line[LINE_SIZE];

    unsigned long total = 0;
    unsigned long status_2xx = 0;
    unsigned long status_3xx = 0;
    unsigned long status_4xx = 0;
    unsigned long status_5xx = 0;
    unsigned long parse_errors = 0;

    if (argc != 2) {
        fprintf(stderr, "使用方法: %s ログファイル\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "r");

    if (fp == NULL) {
        perror("fopen");
        return 1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char ip[IP_SIZE];
        char method[METHOD_SIZE];
        char path[PATH_SIZE];
        char protocol[PROTOCOL_SIZE];
        int status;

        total++;

        /*
         * Apacheログから次を抽出する。
         * IP、HTTPメソッド、パス、HTTPバージョン、ステータスコード
         */
        int result = sscanf(
            line,
            "%45s %*s %*s [%*[^]]] \"%15s %2047s %15[^\"]\" %d",
            ip,
            method,
            path,
            protocol,
            &status
        );

        if (result != 5) {
            parse_errors++;
            continue;
        }

        if (status >= 200 && status <= 299) {
            status_2xx++;
        } else if (status >= 300 && status <= 399) {
            status_3xx++;
        } else if (status >= 400 && status <= 499) {
            status_4xx++;
        } else if (status >= 500 && status <= 599) {
            status_5xx++;
        }

        if (status == 404) {
            printf("[404] IP=%s METHOD=%s PATH=%s\n",
                   ip, method, path);
        }
    }

    fclose(fp);

    printf("\n===== 解析結果 =====\n");
    printf("総ログ行数     : %lu\n", total);
    printf("2xx 成功       : %lu\n", status_2xx);
    printf("3xx リダイレクト: %lu\n", status_3xx);
    printf("4xx クライアントエラー: %lu\n", status_4xx);
    printf("5xx サーバーエラー    : %lu\n", status_5xx);
    printf("解析失敗行     : %lu\n", parse_errors);

    return 0;
}