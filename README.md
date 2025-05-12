#  Giới thiệu Game: Fireboy & Watergirl

Đây là một phiên bản tự làm lại của game nổi tiếng *Fireboy and Watergirl*. Bạn sẽ điều khiển **hai nhân vật cùng lúc**:  
- **Fireboy** chỉ có thể đi qua lửa  
- **Watergirl** chỉ có thể đi qua nước  
  Cả hai cần phối hợp để vượt qua các chướng ngại vật, thu thập kim cương và cùng đến cánh cửa an toàn.

> Cảm hứng gốc từ game: (https://www.youtube.com/watch?v=og0inOdAh8w)

---

##  Mục lục

- [Cách Tải Game](#cách-tải-game)
- [Bắt đầu Game](#bắt-đầu-game)
- [Các thành phần trong game](#các-thành-phần-trong-game)
- [Cách Chơi](#cách-chơi)
- [Chiến Thắng](#chiến-thắng)
- [Đồ Họa](#đồ-họa)
- [Source Code](#source-code)
- [Tính năng khác](#tính-năng-khác)
- [Thông tin hỗ trợ](#thông-tin-hỗ-trợ)
- [Video demo](#video-demo)
---

##  Cách Tải Game

Bước 1: Chọn Code -> Download Zip (6,6MB)
![Screenshot 2025-04-26 014200](https://github.com/user-attachments/assets/ca374a63-03a1-4a37-a05c-32e598f4423f)

Bước 2: Chọn file **fireboy watergirl.cbp**, sau đó ấn F8 để bắt đầu chơi

## Bắt đầu Game

Ấn Enter để vào bắt đầu Game. Trước khi bắt đầu Game, ấn I để đọc Instructions
![image](https://github.com/user-attachments/assets/1a4ce75b-6b96-485c-9c1c-f6d4a4754090)

## Các thành phần trong game

- [Nhân vật(Dành cho hai người chơi)]: Fireboy và Watergirl: Hai nhân vật bạn điều khiển để dành chiến thắng.
![fireboy](https://github.com/user-attachments/assets/5f14cbc6-09c8-4589-b574-28406fefbbe4)
![watergirl](https://github.com/user-attachments/assets/62161a33-07d5-4419-8cb9-c6b3882abf54)
- [Các vùng chất lỏng]:
- Vùng lửa :Fireboy có thể di chuyển vào, watergirl không thể di chuyển vào. Nếu watergirl chạm vào vùng lửa trò chơi kết thúc.

![flake](https://github.com/user-attachments/assets/c489e158-9036-49d2-95e4-979a316e227f)
- Vùng nướ:Watergirl có thể di chuyển vào, watergirl không thể di chuyển vào. Nếu fireboy chạm vào vùng lửa trò chơi kết thúc.
![wlake](https://github.com/user-attachments/assets/e0ccce08-8485-4f40-a8db-75b54f6427b3)
- Vùng độc:Cả hai nhân vật đều không chạm vào được. Nếu chạm vào nhân vật sẽ chết, trò chơi kết thúc.
![glake1](https://github.com/user-attachments/assets/cc7e98e7-0ac9-40fb-b8bf-5f980dcd4df7)
- Map: Nhân vật có thể di chuyển ở các vùng gạch vàng ở các tầng, có thể thực hiện nhảy lên ở các tầng.
![image](https://github.com/user-attachments/assets/56ff9838-7466-4f10-918f-394430a879bd)
- Kim cương: Fireboy ăn được kim cương đỏ, watergirl ăn được kim cương xanh.
![diamond_16x16](https://github.com/user-attachments/assets/94c2fbdf-ee91-4fae-b7e4-bfd4509f2a9f)
- Door:Nhân vật đến cửa để thắng. Cửa xanh cho watergirl, cửa đỏ cho fireboy
![image](https://github.com/user-attachments/assets/ff3db3b0-a0d8-4e90-a393-95a73c1c55a4)


## Cách chơi:
- Di chuyển trong các vùng địa hình cho tới khi tới đích.
- Lưu ý: Fireboy không thể di chuyển vào vùng nước hay vùng độc, watergirl không thể di chuyển vào vùng lửa hay vùng độc.
- Sử dụng các phím A,W,D để di chuyển fireboy, -> <- ^ để di chuyển watergirl.
- Ăn kim cương tương ứng với nhân vật.
- Áp dụng lực nhảy,căn vị trí chính xác nhất để có thể nhảy qua vùng nguy hiểm an toàn.
- Bạn sẽ thắng khi tới được cửa, hãy cố gắng tới cửa trong thời gian sớm nhất! 5 người nhanh nhất sẽ lưu tên trong highscore.

## Chiến thắng:
- Bạn vượt qua toàn bộ thử thách và đến cửa để điền tên: 
![image](https://github.com/user-attachments/assets/4517ee49-6f80-420a-b717-c2baccece3bf)
- Màn hình sẽ hiện cho bạn replay hoặc về menu đầu tiên:
![image](https://github.com/user-attachments/assets/343b94b1-be81-41dc-8246-eef9c53603bb)

## Đồ họa:
- Nhân vật:
![Fireboy]: https://official-fireboy-watergirl.fandom.com/wiki/Fireboy 
![Watergirl]: https://official-fireboy-watergirl.fandom.com/wiki/Watergirl
- Map:Tự vẽ trên Tiled, tham khảo:
https://github.com/pavel-skala/Fireboy-and-Watergirl/blob/main/res/img/maps/level1.png
- Các vùng độc, lửa, nước:Tham khảo từ game gốc, chỉnh sửa lại cho phù hợp.
- Diamond:Tham khảo từ game gốc, chỉnh sửa lại cho phù hợp.
- Door:Tham khảo từ game gốc, chỉnh sửa cho phù hợp.
- Font:
https://official-fireboy-watergirl.fandom.com/wiki/Fonts_Used_In_The_Game
-App hỗ trợ:Tiled
## Source code:
- Thư viện sử dụng:Tmxlite,SDL_image,SDL_ttf,SDL.
- File font:Lưu font liên quan.
- File img:Lưu hình ảnh liên quan đến map và game.
- File tsx:Lưu thông tin về map, các layer trong map.
- File highscore:Lưu điểm cao.
- File cpp,h, cbp: Lưu code game.
-File còn lại:Khởi tạo trong quá trình code dễ dàng hơn.
## Tính năng khác:
- Ở main menu, ấn H để xem kỉ lục qua màn của người chơi.
- Ở main menu, ấn I để xem hướng dẫn chi tiết về cách game vận hành.

## Thông tin hỗ trợ:
- Thư viện ngoài:Thư viện tmxlite
- Cơ chế: Xử lý va chạm theo các layer của map được up lên bằng tiled và tmxlite
- App: Adobe Photoshop
## Video demo:
https://drive.google.com/drive/folders/1pLiIi7SGYPk2XHU9guuvGRc9gBI0XM1C







