# Growtopia-Multibot-Socks5
%100 Free to use and sell 0x44 skid

Example path finding and harvest 
https://user-images.githubusercontent.com/94198465/230380659-ef2a49ff-7ec8-4a79-b6e4-65ee01eb3101.mp4

function harvest()
        for _, tile in pairs(GetTiles()) do
            FindPath(45,23)
            Sleep(150)
            Hit(0,0,18)
            Sleep(50)
            FindPath(46,23)
            Sleep(150)
            Hit(0,0,18)
            Sleep(50)
        end
    end
Sleep(5000)
harvest()
