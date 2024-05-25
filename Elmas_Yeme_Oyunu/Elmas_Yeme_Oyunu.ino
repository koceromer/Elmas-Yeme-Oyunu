#include <LiquidCrystal.h>  // LCD kütüphanesi dahil edildi

// LCD ekranın bağlantı pinleri tanımlandı
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Özel karakterler tanımlandı
byte karakter[] = {0, 4, 14, 31, 31, 14, 10, 0}; // Karakterin şekli
byte canavar[] = {0, 14, 21, 31, 31, 21, 31, 0}; // Canavarın şekli
byte elmasKucuk[] = {0, 4, 14, 14, 14, 4, 0, 0}; // Küçük elmas
byte elmasOrta[] = {0, 10, 31, 31, 14, 4, 0, 0}; // Orta elmas
byte elmasBuyuk[] = {0, 14, 31, 31, 31, 14, 0, 0}; // Büyük elmas
byte bomba[] = {0, 10, 31, 31, 31, 10, 0, 0}; // Bomba şekli
byte kalp[] = {0, 10, 31, 31, 31, 14, 4, 0}; // Kalp şekli

// Oyun değişkenleri tanımlandı
int x, y, skor, can;
int elmas_x, elmas_y, elmas_puan, bomba_x, bomba_y, kalp_x, kalp_y, canavar_x, canavar_y;
boolean elmas_yedi_mi, bomba_yedi_mi, kalp_yedi_mi;
unsigned long sonCanavarZamani, kalpSonZaman, elmasSonZaman, bombaSonZaman;
const unsigned long canavarGelisSuresi = 15000; // Canavarın geliş süresi
const unsigned long objeYokOlmaSuresi = 10000; // Obje yok olma süresi
const unsigned long kalpCikisSuresi = 30000; // Kalp çıkış süresi

// Tuş tanımlamaları
#define sag 0
#define ust 1
#define alt 2
#define sol 3
#define secim 4
#define yok 5

// Tuş okuma fonksiyonu
int tus_oku() {
  int okunan = analogRead(0);  // A0 pininden analog veri okunuyor
  if (okunan > 1000) return yok; // Hiçbir tuşa basılmadı
  if (okunan < 50) return sag; // Sağ tuş
  if (okunan < 195) return ust; // Üst tuş
  if (okunan < 380) return alt; // Alt tuş
  if (okunan < 555) return sol; // Sol tuş
  if (okunan < 790) return secim; // Seçim tuşu
  return yok;
}

// Rastgele elmas oluşturma fonksiyonu
void rastgele_elmas() {
  elmas_x = random(14); // Elmasın x koordinatı rastgele seçilir
  elmas_y = random(2); // Elmasın y koordinatı rastgele seçilir
  int rastgele_puan = random(3); // Elmasın puanı rastgele belirlenir
  switch (rastgele_puan) {
    case 0:
      elmas_puan = 1;
      lcd.createChar(2, elmasKucuk); // Küçük elmas oluşturulur
      break;
    case 1:
      elmas_puan = 3;
      lcd.createChar(2, elmasOrta); // Orta elmas oluşturulur
      break;
    case 2:
      elmas_puan = 5;
      lcd.createChar(2, elmasBuyuk); // Büyük elmas oluşturulur
      break;
  }
  lcd.setCursor(elmas_x, elmas_y); // Elmasın konumu belirlenir
  lcd.write(byte(2)); // Elmas ekrana yazdırılır
  elmas_yedi_mi = false; // Elmas yenildi mi bilgisi false yapılır
  elmasSonZaman = millis(); // Elmasın oluştuğu zaman kaydedilir
}

// Rastgele bomba oluşturma fonksiyonu
void rastgele_bomba() {
  bomba_x = random(14); // Bombanın x koordinatı rastgele seçilir
  bomba_y = random(2); // Bombanın y koordinatı rastgele seçilir
  lcd.setCursor(bomba_x, bomba_y); // Bombanın konumu belirlenir
  lcd.write(byte(3)); // Bomba ekrana yazdırılır
  bomba_yedi_mi = false; // Bomba yenildi mi bilgisi false yapılır
  bombaSonZaman = millis(); // Bombanın oluştuğu zaman kaydedilir
}

// Rastgele kalp oluşturma fonksiyonu
void rastgele_kalp() {
  kalp_x = random(14); // Kalbin x koordinatı rastgele seçilir
  kalp_y = random(2); // Kalbin y koordinatı rastgele seçilir
  lcd.setCursor(kalp_x, kalp_y); // Kalbin konumu belirlenir
  lcd.write(byte(4)); // Kalp ekrana yazdırılır
  kalp_yedi_mi = false; // Kalp yenildi mi bilgisi false yapılır
  kalpSonZaman = millis(); // Kalbin oluştuğu zaman kaydedilir
}

// Rastgele canavar oluşturma fonksiyonu
void rastgele_canavar() {
  canavar_x = 14; // Canavarın başlangıç x koordinatı
  canavar_y = random(2); // Canavarın y koordinatı rastgele seçilir
}

// Giriş animasyonu fonksiyonu
void giris_animasyonu() {
  lcd.clear(); // LCD ekran temizlenir
  for (int i = 0; i < 3; i++) {
    lcd.setCursor(0, 0); lcd.print(" #  *  # * #  "); // Yıldız şekilleri ekrana yazdırılır
    lcd.setCursor(0, 1); lcd.print("*  # * #  * # "); // Yıldız şekilleri ekrana yazdırılır
    delay(300); lcd.clear(); delay(300); // Ekran temizlenir ve beklenir
    lcd.setCursor(0, 0); lcd.print(" *  # *  # *  "); // Yıldız şekilleri ekrana yazdırılır
    lcd.setCursor(0, 1); lcd.print(" #  * #  * #  "); // Yıldız şekilleri ekrana yazdırılır
    delay(300); lcd.clear(); delay(300); // Ekran temizlenir ve beklenir
  }
}

// Menü gösterme fonksiyonu
void menu_goster() {
  lcd.clear(); // LCD ekran temizlenir
  lcd.setCursor(0, 0); lcd.print("Baslamak icin"); // Menü mesajı ekrana yazdırılır
  lcd.setCursor(0, 1); lcd.print("bir tusa basin"); // Menü mesajı ekrana yazdırılır
  while (tus_oku() == yok); // Tuşa basılmasını bekler
  lcd.clear(); // LCD ekran temizlenir
}

// Oyun bitti fonksiyonu
void oyun_bitti() {
  lcd.clear(); // LCD ekran temizlenir
  lcd.setCursor(4, 0); lcd.print("GAME OVER"); // Game Over mesajı ekrana yazdırılır
  lcd.setCursor(0, 1); lcd.print("Tusa basin"); // Tuşa basın mesajı ekrana yazdırılır
  while (tus_oku() == yok); // Tuşa basılmasını bekler
  oyunu_sifirla(); // Oyun sıfırlama fonksiyonu çağrılır
  menu_goster(); // Menü gösterme fonksiyonu çağrılır
}

// Oyunu sıfırlama fonksiyonu
void oyunu_sifirla() {
  x = y = skor = 0; // Karakterin x ve y konumu, skor sıfırlanır
  can = 3; // Can sayısı 3 yapılır
  elmas_yedi_mi = bomba_yedi_mi = kalp_yedi_mi = true; // Elmas, bomba, kalp yenildi mi bilgileri true yapılır
  sonCanavarZamani = kalpSonZaman = elmasSonZaman = bombaSonZaman = 0; // Zamanlayıcılar sıfırlanır
  rastgele_elmas(); // Rastgele elmas oluşturma fonksiyonu çağrılır
  rastgele_bomba(); // Rastgele bomba oluşturma fonksiyonu çağrılır
  rastgele_kalp(); // Rastgele kalp oluşturma fonksiyonu çağrılır
  rastgele_canavar(); // Rastgele canavar oluşturma fonksiyonu çağrılır
}

// Başlangıç ayarları
void setup() {
  lcd.begin(16, 2); // LCD ekranın başlatılması
  lcd.createChar(0, karakter); lcd.createChar(1, karakter); // Karakterlerin tanımlanması
  lcd.createChar(3, bomba); lcd.createChar(4, kalp); lcd.createChar(5, canavar); // Bomba, kalp, canavar karakterlerinin tanımlanması

  giris_animasyonu(); // Giriş animasyonu fonksiyonu çağrılır
  menu_goster(); // Menü gösterme fonksiyonu çağrılır
  oyunu_sifirla(); // Oyun sıfırlama fonksiyonu çağrılır
}

// Ana döngü
void loop() {
  static int oncekiCanavar_x = canavar_x, oncekiCanavar_y = canavar_y; // Önceki canavar koordinatları

  lcd.setCursor(x, y); lcd.write(byte(0)); delay(350); // Karakter ekrana yazdırılır
  lcd.setCursor(x, y); lcd.write(byte(1)); delay(200); // Karakter animasyonu ekrana yazdırılır
  lcd.setCursor(x, y); lcd.print(" "); // Karakter ekrandan silinir

  // Tuş okuma ve hareket kontrolü
  switch (tus_oku()) {
    case sag: if (x < 13) x++; break; // Sağ tuşa basılırsa x artırılır
    case sol: if (x > 0) x--; break; // Sol tuşa basılırsa x azaltılır
    case ust: y = 0; break; // Üst tuşa basılırsa y sıfırlanır
    case alt: y = 1; break; // Alt tuşa basılırsa y bir yapılır
  }

  // Elmas süresi kontrolü ve yenilenmesi
  if (elmas_yedi_mi && millis() - elmasSonZaman > objeYokOlmaSuresi) {
    rastgele_elmas(); // Rastgele elmas oluşturma fonksiyonu çağrılır
  }

  // Bomba süresi kontrolü ve yenilenmesi
  if (bomba_yedi_mi && millis() - bombaSonZaman > objeYokOlmaSuresi) {
    rastgele_bomba(); // Rastgele bomba oluşturma fonksiyonu çağrılır
  }

  // Kalp süresi kontrolü ve yenilenmesi
  if (kalp_yedi_mi && millis() - kalpSonZaman > kalpCikisSuresi) {
    rastgele_kalp(); // Rastgele kalp oluşturma fonksiyonu çağrılır
  }

  // Karakterin elmas, bomba ve kalp ile etkileşimi
  if (x == elmas_x && y == elmas_y) {
    elmas_yedi_mi = true; skor += elmas_puan; lcd.setCursor(elmas_x, elmas_y); lcd.print(" "); // Elmas yenildi, skor artırıldı ve elmas silindi
    rastgele_elmas();
  }
  if (x == bomba_x && y == bomba_y) {
    bomba_yedi_mi = true; can--; lcd.setCursor(bomba_x, bomba_y); lcd.print(" "); // Bomba yenildi, can azaltıldı ve bomba silindi
    rastgele_bomba();
    if (can == 0) { oyun_bitti(); return; } // Can sıfırsa oyun bitti
  }
  if (x == kalp_x && y == kalp_y) {
    kalp_yedi_mi = true; can++; lcd.setCursor(kalp_x, kalp_y); lcd.print(" "); // Kalp yenildi, can artırıldı ve kalp silindi
    rastgele_kalp();
  }

  // Canavarın hareketi ve süre kontrolü
  if (millis() - sonCanavarZamani >= canavarGelisSuresi) {
    sonCanavarZamani = millis();
    lcd.setCursor(oncekiCanavar_x, oncekiCanavar_y); lcd.print("  "); // Önceki canavar silinir
    rastgele_canavar(); // Rastgele canavar oluşturma fonksiyonu çağrılır
  } else {
    lcd.setCursor(oncekiCanavar_x, oncekiCanavar_y); lcd.print("  "); // Önceki canavar silinir
    //rastgele_canavar();
  }

  oncekiCanavar_x = canavar_x; oncekiCanavar_y = canavar_y; // Canavarın yeni koordinatları

  canavar_x--; // Canavar sola hareket eder
  if (canavar_x < 0) {
    canavar_x = 15; canavar_y = random(2); // Canavarın x sıfırın altına düşerse başa döner
  }

  // Karakterin canavar ile çarpışma kontrolü    
  if (x == canavar_x && y == canavar_y) {
    oyun_bitti(); return; // Karakter canavara çarptıysa oyun biter
  }

  // Elmas, bomba ve kalbin ekrandan silinmesi ve süre kontrolü
  if (millis() - elmasSonZaman >= objeYokOlmaSuresi && !elmas_yedi_mi) {
    lcd.setCursor(elmas_x, elmas_y); lcd.print(" "); elmas_yedi_mi = true; // Elmas süresi dolduysa silinir
  }
  if (millis() - bombaSonZaman >= objeYokOlmaSuresi && !bomba_yedi_mi) {
    lcd.setCursor(bomba_x, bomba_y); lcd.print(" "); bomba_yedi_mi = true; // Bomba süresi dolduysa silinir
  }
  if (millis() - kalpSonZaman >= kalpCikisSuresi && !kalp_yedi_mi) {
    lcd.setCursor(kalp_x, kalp_y); lcd.print(" "); kalp_yedi_mi = true; // Kalp süresi dolduysa silinir
  }

  // Can ve skorun ekrana yazılması
  lcd.setCursor(15, 0); lcd.print(can); // Can sayısı ekrana yazılır
  lcd.setCursor(15, 1); if (skor < 10) lcd.print(skor); else lcd.setCursor(14, 1); lcd.print(skor); // Skor ekrana yazılır

  // Karakterin tekrar çizilmesi
  lcd.setCursor(x, y); lcd.write(byte(0)); // Karakter ekrana yazılır
  if (!elmas_yedi_mi) { lcd.setCursor(elmas_x, elmas_y); lcd.write(byte(2)); } // Elmas ekrana yazılır
  if (!bomba_yedi_mi) { lcd.setCursor(bomba_x, bomba_y); lcd.write(byte(3)); } // Bomba ekrana yazılır
  if (!kalp_yedi_mi) { lcd.setCursor(kalp_x, kalp_y); lcd.write(byte(4)); } // Kalp ekrana yazılır
  lcd.setCursor(canavar_x, canavar_y); lcd.write(byte(5)); // Canavar ekrana yazılır

  // Skor kontrolü
  if (skor >= 99) {
    lcd.clear(); // Ekran temizlenir
    lcd.setCursor(5, 0); lcd.print("WINNER"); // Kazanma mesajı yazılır
    delay(2000); // 2 saniye beklenir
    oyunu_sifirla(); // Oyun sıfırlama fonksiyonu çağrılır
  }
}
