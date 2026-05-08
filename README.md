🎓 IoT Tabanlı Gerçek Zamanlı Sınıf Mevcut Takip Sistemi
Bu proje, eğitim kurumlarında sınıf mevcudunun manuel yöntemlerle sayılması yerine, giriş ve çıkış noktalarına yerleştirilen sensörler aracılığıyla hatasız ve anlık olarak takip edilmesini sağlayan bir IoT (Nesnelerin İnterneti) çözümüdür.
⚠️ Sorun Tanımı Geleneksel eğitim ortamlarında sınıf mevcudunun manuel olarak sayılması hem ders vaktinden çalmakta hem de insan kaynaklı hata payı barındırmaktadır. Sınıf doluluk oranının anlık olarak izlenememesi, modern eğitim teknolojileri (EdTech) ve okul yönetimsel verimliliği açısından bir eksiklik oluşturmaktadır.
✅ Çözüm: Otomatik ve Uzaktan TakipGeliştirilen sistem, sınıf kapısına yerleştirilen iki adet ultrasonik mesafe sensörü ile öğrenci hareketlerini ve yönünü algılar. ESP8266 mikrodenetleyicisi sayesinde hesaplanan güncel kişi sayısı, Wi-Fi protokolü üzerinden öğretmenin ekranına anlık olarak iletilir. Bu sayede fiziksel sayıma gerek kalmadan dijital, güncel ve uzaktan erişilebilir bir takip mekanizması sağlanır.
🛠️ Proje Kısımları ve Teknik Yapı
1. Donanım MimarisiProjenin merkezinde Wi-Fi yeteneklerine sahip ESP8266 kontrol kartı yer almaktadır. Kişi algılama ve mesafe ölçümü işlemi için iki adet HC-SR04 ultrasonik sensör kullanılmıştır.
   
Bileşen,Görev,Trig Pin,Echo Pin
Sensör A,Giriş Yönü Algılama,2,3
Sensör B,Çıkış Yönü Algılama,4,5
2.Çalışma Mantığı (Yön Algılama Algoritması)
Sistem, iki sensörün tetiklenme sırasını takip ederek yön tayini yapar:

Giriş (+1): Hareket önce A sensörü, ardından B sensörü tarafından tespit edilirse sistem sınıfa bir öğrencinin girdiğini algılar ve toplam sayıyı artırır.

Çıkış (-1): Hareket önce B sensörü, ardından A sensörü tarafından tespit edilirse sistem bir öğrencinin sınıftan çıktığını algılar ve toplam sayıyı azaltır.

3. IoT Entegrasyonu ve Veri İletimi
ESP8266 üzerindeki yazılım, her hareket algılandığında güncel mevcut sayısını kablosuz ağ üzerinden sunucuya/panele iletir. Bu veri akışı sayesinde öğretmen ekranındaki panel, sınıftaki gerçek durumla senkronize bir şekilde otomatik olarak güncellenir.
🚀 Kurulum ve Kullanım
Donanımı yukarıdaki pin tablosuna uygun şekilde ESP8266'ya bağlayın.

giriş_çıkış_sayaç.ino dosyasını Arduino IDE kullanarak kartınıza yükleyin.

Sensörleri kapı girişine, aralarında kısa bir mesafe olacak şekilde (A dışa, B içe bakacak şekilde) monte edin.

Bağlı olan dijital panel üzerinden anlık öğrenci verilerini takip etmeye başlayın.
