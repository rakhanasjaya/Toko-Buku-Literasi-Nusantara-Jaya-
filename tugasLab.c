/*
 * Toko Buku "Literasi Nusantara Jaya"
 *
 *   - Bagian atas (struct dan variabel global) adalah kontrak tim.
 *   - Format databuku.txt : kode|nama|jenis|harga
 *   - Format history.txt  : kode|jumlah|total
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ====================== KONTRAK TIM ====================== */
#define MAX_BUKU 100
#define MAX_HISTORY 500
#define FILE_BUKU "databuku.txt"
#define FILE_HISTORY "history.txt"

#define LEN_KODE 10
#define LEN_NAMA 100
#define LEN_JENIS 50

typedef struct
{
    char kode[LEN_KODE];
    char nama[LEN_NAMA];
    char jenis[LEN_JENIS];
    double harga;
} Buku;

typedef struct
{
    char kode[LEN_KODE];
    int jumlah;
    double total;
} History;

/* Data di memori: satu-satunya sumber data selama program berjalan. */
Buku daftarBuku[MAX_BUKU];
int jumlahBuku = 0;
History daftarHistory[MAX_HISTORY];
int jumlahHistory = 0;

/* Menampilkan prompt lalu membaca satu baris (spasi diperbolehkan). */
void bacaBaris(const char *prompt, char *buf, int size)
{
    printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL)
    {
        buf[0] = '\0';
        return;
    }
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
    {
        buf[len - 1] = '\0';
    }
    else
    {
        int c; /* input lebih panjang dari buffer: buang sisanya */
        while ((c = getchar()) != '\n' && c != EOF)
        {
        }
    }
}

/* Membaca integer. Return 1 jika valid, 0 jika bukan angka. */
int bacaInt(const char *prompt, int *out)
{
    char buf[32], *end;
    bacaBaris(prompt, buf, sizeof(buf));
    long v = strtol(buf, &end, 10);
    if (end == buf || *end != '\0')
        return 0;
    *out = (int)v;
    return 1;
}

/* Membaca double. Return 1 jika valid, 0 jika bukan angka. */
int bacaDouble(const char *prompt, double *out)
{
    char buf[32], *end;
    bacaBaris(prompt, buf, sizeof(buf));
    double v = strtod(buf, &end);
    if (end == buf || *end != '\0')
        return 0;
    *out = v;
    return 1;
}

/* Meminta index sampai valid (minimal 1, maksimal max). Dipakai Menu 4 dan 5. */
int inputIndex(int max)
{
    char prompt[64];
    int idx;
    snprintf(prompt, sizeof(prompt), "Masukkan index (1-%d): ", max);
    while (!bacaInt(prompt, &idx) || idx < 1 || idx > max)
    {
        printf("Input tidak valid.\n");
    }
    return idx;
}

/* Karakter '|' dipakai sebagai pemisah di file, jadi diganti agar file tidak rusak. */
void hapusPemisah(char *s)
{
    for (; *s; s++)
    {
        if (*s == '|')
            *s = '-';
    }
}

/* ====================== BAGIAN ANGGOTA A: STORAGE ====================== */

/* Membaca kedua file ke array. Jika file belum ada, array dibiarkan kosong (tidak error). */
void loadData(void)
{
    char baris[256];
    FILE *f;

    jumlahBuku = 0;
    jumlahHistory = 0;

    f = fopen(FILE_BUKU, "r");
    if (f != NULL)
    {
        while (jumlahBuku < MAX_BUKU && fgets(baris, sizeof(baris), f) != NULL)
        {
            Buku b;
            if (sscanf(baris, "%9[^|]|%99[^|]|%49[^|]|%lf",
                       b.kode, b.nama, b.jenis, &b.harga) == 4)
            {
                daftarBuku[jumlahBuku++] = b;
            }
        }
        fclose(f);
    }

    f = fopen(FILE_HISTORY, "r");
    if (f != NULL)
    {
        while (jumlahHistory < MAX_HISTORY && fgets(baris, sizeof(baris), f) != NULL)
        {
            History h;
            if (sscanf(baris, "%9[^|]|%d|%lf", h.kode, &h.jumlah, &h.total) == 3)
            {
                daftarHistory[jumlahHistory++] = h;
            }
        }
        fclose(f);
    }
}

/* Menulis seluruh isi array ke file (dipanggil di Menu 6). File dibuat otomatis jika belum ada. */
void saveData(void)
{
    int ok = 1;
    FILE *f = fopen(FILE_BUKU, "w");

    if (f == NULL)
    {
        printf("Gagal membuat %s\n", FILE_BUKU);
        ok = 0;
    }
    else
    {
        for (int i = 0; i < jumlahBuku; i++)
        {
            fprintf(f, "%s|%s|%s|%.2f\n", daftarBuku[i].kode, daftarBuku[i].nama,
                    daftarBuku[i].jenis, daftarBuku[i].harga);
        }
        fclose(f);
    }

    f = fopen(FILE_HISTORY, "w");
    if (f == NULL)
    {
        printf("Gagal menulis %s\n", FILE_HISTORY);
        ok = 0;
    }
    else
    {
        for (int i = 0; i < jumlahHistory; i++)
        {
            fprintf(f, "%s|%d|%.2f\n", daftarHistory[i].kode,
                    daftarHistory[i].jumlah, daftarHistory[i].total);
        }
        fclose(f);
    }

    if (ok)
        printf("Data berhasil disimpan.\n");
}

/* Membuat kode otomatis BK001, BK002, dst.
 * Diambil dari angka kode terbesar + 1 (bukan sekadar jumlah + 1) agar tidak
 * duplikat setelah ada buku yang dihapus. */
void generateKode(char *kode)
{
    int maks = 0;
    for (int i = 0; i < jumlahBuku; i++)
    {
        if (strlen(daftarBuku[i].kode) > 2)
        {
            int n = atoi(daftarBuku[i].kode + 2);
            if (n > maks && n < 999999)
                maks = n;
        }
    }
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "BK%03d", maks + 1);
    strncpy(kode, tmp, LEN_KODE - 1);
    kode[LEN_KODE - 1] = '\0';
}

/* Mencari buku berdasarkan kode. Return index di daftarBuku, atau -1 jika tidak ada. */
int cariBuku(const char *kode)
{
    for (int i = 0; i < jumlahBuku; i++)
    {
        if (strcmp(daftarBuku[i].kode, kode) == 0)
            return i;
    }
    return -1;
}

/* Menu 1: meminta nama, jenis, harga; kode dibuat otomatis; data ditambah ke array. */
void inputBuku(void)
{
    Buku b;

    if (jumlahBuku >= MAX_BUKU)
    {
        printf("Data buku penuh (maksimal %d).\n", MAX_BUKU);
        return;
    }

    bacaBaris("Nama buku  : ", b.nama, sizeof(b.nama));
    bacaBaris("Jenis buku : ", b.jenis, sizeof(b.jenis));
    hapusPemisah(b.nama);
    hapusPemisah(b.jenis);
    if (b.nama[0] == '\0' || b.jenis[0] == '\0')
    {
        printf("Nama dan jenis buku tidak boleh kosong.\n");
        return;
    }

    while (!bacaDouble("Harga buku : ", &b.harga) || b.harga <= 0)
    {
        printf("Harga harus berupa angka lebih dari 0.\n");
    }

    generateKode(b.kode);
    daftarBuku[jumlahBuku++] = b;
    printf("Buku berhasil ditambahkan dengan kode %s.\n", b.kode);
}

/* Menu 3: menampilkan seluruh buku dari array (hanya tampil, tanpa input/clear screen).
 * Return jumlah buku yang ditampilkan, 0 jika kosong. Dipakai juga oleh deleteBuku(). */
int viewBuku(void)
{
    if (jumlahBuku == 0)
    {
        printf("Data buku belum ada.\n");
        return 0;
    }
    printf("\n%-4s %-8s %-30s %-15s %12s\n", "No", "Kode", "Nama", "Jenis", "Harga");
    for (int i = 0; i < jumlahBuku; i++)
    {
        printf("%-4d %-8s %-30.30s %-15.15s %12.2f\n", i + 1, daftarBuku[i].kode,
               daftarBuku[i].nama, daftarBuku[i].jenis, daftarBuku[i].harga);
    }
    return jumlahBuku;
}

// TODO: Anggota selanjutnya
void viewHistory(void){
    if (jumlahHistory == 0)
    {
        printf("\nBelum ada riwayat transaksi.\n");
        return;
    }

    printf("\n========== HISTORY TRANSAKSI ==========\n");
    printf("%-4s %-10s %-15s %-15s\n",
           "No", "Kode Buku", "Jumlah Terjual", "Total Harga");

    for (int i = 0; i < jumlahHistory; i++)
    {
        printf("%-4d %-10s %-15d Rp %-12.2f\n",
               i + 1,
               daftarHistory[i].kode,
               daftarHistory[i].jumlah,
               daftarHistory[i].total);
    }

    printf("========================================\n");
}
void deleteHistory(void){
    if (jumlahHistory == 0)
    {
        printf("\nBelum ada riwayat transaksi yang bisa dihapus.\n");
        return;
    }

    printf("\n========== DAFTAR HISTORY ==========\n");
    printf("%-4s %-10s %-15s %-15s\n",
           "No", "Kode Buku", "Jumlah Terjual", "Total Harga");

    for (int i = 0; i < jumlahHistory; i++)
    {
        printf("%-4d %-10s %-15d Rp %-12.2f\n",
               i + 1,
               daftarHistory[i].kode,
               daftarHistory[i].jumlah,
               daftarHistory[i].total);
    }

    printf("====================================\n");

    int index = inputIndex(jumlahHistory);

    for (int i = index - 1; i < jumlahHistory - 1; i++)
    {
        daftarHistory[i] = daftarHistory[i + 1];
    }

    jumlahHistory--;

    printf("Data history berhasil dihapus.\n");
}
void inputTransaksi(void) {
    if (jumlahBuku == 0)
    {
        printf("\nBelum ada data buku. Silakan input buku terlebih dahulu.\n");
        return;
    }

    if (jumlahHistory >= MAX_HISTORY)
    {
        printf("\nData history sudah penuh.\n");
        return;
    }

    char kode[LEN_KODE];
    int jumlah;
    int indexBuku;

    printf("\n========== INPUT TRANSAKSI ==========\n");

    bacaBaris("Kode buku    : ", kode, sizeof(kode));

    indexBuku = cariBuku(kode);

    if (indexBuku == -1)
    {
        printf("Kode buku tidak ditemukan.\n");
        return;
    }

    do
    {
        if (!bacaInt("Jumlah terjual: ", &jumlah) || jumlah <= 0)
        {
            printf("Jumlah terjual harus berupa angka lebih dari 0.\n");
            jumlah = 0;
        }
    } while (jumlah <= 0);

    History h;

    strcpy(h.kode, daftarBuku[indexBuku].kode);
    h.jumlah = jumlah;
    h.total = jumlah * daftarBuku[indexBuku].harga;

    daftarHistory[jumlahHistory++] = h;

    printf("\n========== DETAIL TRANSAKSI ==========\n");
    printf("Kode buku     : %s\n", daftarBuku[indexBuku].kode);
    printf("Nama buku     : %s\n", daftarBuku[indexBuku].nama);
    printf("Harga satuan  : Rp %.2f\n", daftarBuku[indexBuku].harga);
    printf("Jumlah terjual: %d\n", h.jumlah);
    printf("Total harga   : Rp %.2f\n", h.total);
    printf("Transaksi berhasil dicatat.\n");
}

void deleteBuku(void) { printf("[TODO ] Menu 5 Delete Buku\n"); }
void menuSort(void) { printf("[TODO ] Menu 8 Sort Buku\n"); }

/* Menampilkan menu utama. */
static void tampilMenu(void)
{
    printf("\n===== TOKO BUKU LITERASI NUSANTARA JAYA =====\n");
    printf("1. Input Data Buku Baru\n");
    printf("2. View History Transaksi Penjualan\n");
    printf("3. View Buku\n");
    printf("4. Delete History\n");
    printf("5. Delete Buku\n");
    printf("6. Exit (simpan & keluar)\n");
    printf("7. Input Transaksi Penjualan\n");
    printf("8. Sort Buku\n");
}

/* Alur utama: load data, loop menu, simpan saat Exit. */
int main(void)
{
    int pilihan = 0;

    loadData();

    do
    {
        tampilMenu();
        if (!bacaInt("Pilihan: ", &pilihan))
        {
            printf("Masukkan angka 1-8.\n");
            pilihan = 0;
            continue;
        }
        switch (pilihan)
        {
        case 1:
            inputBuku();
            break;
        case 2:
            viewHistory();
            break;
        case 3:
            viewBuku();
            break;
        case 4:
            deleteHistory();
            break;
        case 5:
            deleteBuku();
            break;
        case 6:
            saveData();
            break;
        case 7:
            inputTransaksi();
            break;
        case 8:
            menuSort();
            break;
        default:
            printf("Pilihan tidak valid.\n");
        }
    } while (pilihan != 6);

    return 0;
}