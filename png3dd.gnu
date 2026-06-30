if (exist("i") == 0 || i < 0) i = 0
set pm3d map
set border 1
unset border
set noxtics
set noytics
set format x""
set format y""
set size square
set palette rgbformula 22,13,-31
#set cbrange[0:0.7]
#set palette gray
#set palette negative #反転

do for [i=0:25940000:10000]{       #[i=初期ステップ数:最終ステップ:飛んでいるデータ数]
    i2 = i/10000 #ファイル名が飛んでいる場合
    filename = sprintf("./png/net_%05d.png",i2) #ディレクトリpngを作って動かす
    step = sprintf("\"uOri_rogc05chi10k3\"   time = %f",i*1/100000.0)
    set title step
    dataname = sprintf("./Ori_rogc05chi10k3/data%d.txt",i) #変更
    
    set term png
    set output filename
    splot dataname using 1:2:3 with pm3d notitle
    set output

    print "Processed: ", filename
}

#gnuplotで  load "png3dd.gnu"  で2次元データをpng画像に.
#アニメーションは端末で↓
# $ ffmpeg -f image2 -r 40 -i ./png/net_%05d.png -vcodec libx264 -pix_fmt yuv420p animation.mp4
