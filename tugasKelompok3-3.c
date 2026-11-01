#include <stdio.h>
#include <string.h>

#define MAKS 1000

struct Siswa
{
    char nama[50];
    int umur;
    char noReg[20];
};

int main()
{
    struct Siswa siswa[MAKS];
    int totalSiswa = 0;
    int i;
    char cari[50];

    /* Buka file siswa.txt */
    FILE *file = fopen("siswa.txt", "r");

    /* Cek apakah file ada? */
    if (file == NULL)
    {
        printf("Error: file siswa.txt tidak ditemukan.\n");
        return 1;
    }

    /* Baca per baris ke array siswa[], n = n + 1 */
    while (totalSiswa < MAKS &&
           fscanf(file, "%49s %d %19s", siswa[totalSiswa].nama, &siswa[totalSiswa].umur, siswa[totalSiswa].noReg) == 3)
    {
        totalSiswa++;
    }
    fclose(file);

    /* Input nama yang dicari */
    printf("Masukkan nama siswa yang dicari: ");
    scanf("%49s", cari);

    /* Mulai linear search */
    i = 0;
    while (i < totalSiswa)
    {
        if (strcmp(siswa[i].nama, cari) == 0)
        {
            printf("Data ditemukan:\n");
            printf("Nama           : %s\n", siswa[i].nama);
            printf("Umur           : %d\n", siswa[i].umur);
            printf("No. Registrasi : %s\n", siswa[i].noReg);
            return 0;
        }
        i = i + 1;
    }

    printf("Data yang kamu cari tidak dapat ditemukan.\n");
    return 0;
}