🎓 IoT Tabanlı Gerçek Zamanlı Sınıf Mevcut Takip Sistemi
Bu proje, eğitim kurumlarında sınıf mevcudunun manuel yöntemlerle sayılması yerine, giriş ve çıkış noktalarına yerleştirilen sensörler aracılığıyla hatasız ve anlık olarak takip edilmesini sağlayan bir IoT (Nesnelerin İnterneti) çözümüdür.
⚠️ Sorun Tanımı:Geleneksel eğitim ortamlarında sınıf mevcudunun manuel olarak sayılması hem ders vaktinden çalmakta hem de insan kaynaklı hata payı barındırmaktadır. Sınıf doluluk oranının anlık olarak izlenememesi, modern eğitim teknolojileri (EdTech) ve okul yönetimsel verimliliği açısından bir eksiklik oluşturmaktadır.
✅ Çözüm: Otomatik ve Uzaktan TakipGeliştirilen sistem, sınıf kapısına yerleştirilen iki adet ultrasonik mesafe sensörü ile öğrenci hareketlerini ve yönünü algılar.
Arduino Uno mikrodenetleyicisi sayesinde hesaplanan güncel kişi sayısı, eş zamanlı olarak hem fiziksel I2C LCD ekran üzerinde hem de USB üzerinden Seri Port aracılığıyla bilgisayar ekranında görüntülenir.
🛠️ Proje Kısımları ve Teknik Yapı ve Donanım Mimarisi
Projenin merkezinde Arduino Uno kontrol kartı yer almaktadır. Kişi algılama işlemi için iki adet HC-SR04 ultrasonik sensör kullanılmıştır.
Bileşen Görev Bağlantı Pinleri 
Sensör A Giriş Yönü Algılama Trig: 2, Echo: 3 
Sensör B Çıkış Yönü Algılama Trig: 4, Echo: 5 
I2C LCD Bilgi Ekranı 
SDA: A4, SCL: A5 2. 
Çalışma Mantığı (Yön Algılama Algoritması)
Sistem, iki sensörün tetiklenme sırasını takip ederek yön tayini yapar:
Giriş (+1): Hareket önce A sensörü, ardından B sensörü tarafından tespit edilirse sistem sınıfa bir öğrencinin girdiğini algılar.
Çıkış (-1): Hareket önce B sensörü, ardından A sensörü tarafından tespit edilirse sistem bir öğrencinin sınıftan çıktığını algılar.
