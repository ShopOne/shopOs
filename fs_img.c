#include <stdio.h>
#include <string.h>

struct FS_FILEINFO {
    int size, addr;
    unsigned char name[24];
};

int main(int argc, char **argv)
/* argv[1] : 出力ファイル名, argv[2] : IPLファイル名, argv[3～] : ファイル名 */
{
    FILE *fp;
    unsigned char img[1440 * 1024];
    int i, j;
    struct FS_FILEINFO *myfs = (struct FS_FILEINFO *) (img + 0x000200);

    if (argc < 4) {printf("err\n"); return 1; /* エラー終了 */ }

    /* IPL読み込み */
    fp = fopen(argv[2], "rb");
    if (fp == 0) { return 1; }
    fread(img, 1, 512, fp);
    fclose(fp);
    for (i = 512; i < 1440 * 1024; i++) {
        img[i] = 0;
    }

    /* ファイルコピー */
    j = 0x002400;
    for (i = 3; i < argc; i++) {
        myfs[i - 3].addr = j;
        strncpy((char*)myfs[i - 3].name, argv[i], 24); /* 文字列のコピー */
        fp = fopen(argv[i], "rb");
        if (fp == 0) { return 1; }
        myfs[i - 3].size = fread(img + j, 1, 1440 * 1024 - j, fp);
        fclose(fp);
        j = (j + myfs[i - 3].size + 0xf) & ~0xf; /* なんとなく16バイト単位に切り上げ */
        if (j == 1440 * 1024) { break; }
    }
    myfs[argc - 3].size = -1; /* 終端マーク */
    myfs[argc - 3].addr = j; 

    /* ディスクイメージ出力 */
    fp = fopen(argv[1], "wb");
    fwrite(img, 1, 1440 * 1024, fp);
    fclose(fp);
    return 0;
}
