# Numerical Simulation of Pattern Formation (ミツバチ営巣の異方性モデル化)

大学院の研究において実装した、反応拡散系（SDBモデル等）の数値シミュレーションプログラムです。
自然現象を記述した偏微分方程式を離散化し、C言語を用いて数値計算アルゴリズムをゼロから実装しています。

## 出力結果（Gnuplotによる可視化）
計算結果のデータをGnuplotで描画し、パターンの形成過程を動画化しました。
パラメータの変動によって、システムが動的にパターンを変化・形成していく様子を確認できます。

### 変数 u, v, w のシミュレーション推移
（※変数 $u$ はx軸方向、$v$ はy軸方向への影響度や濃度を示しています）
| u View (Heatmap) | v View (Surface Plot) | w View (Surface Plot) |
| :---: | :---: | :---: |
| <video src="https://github.com/user-attachments/assets/65e19e5b..." width="100%" autoplay loop muted></video> | <video src="https://github.com/user-attachments/assets/384578c2..." width="100%" autoplay loop muted></video> | <video src="https://github.com/user-attachments/assets/db9bde23..." width="100%" autoplay loop muted></video> |

## 直面している技術的課題と現在のアプローチ

**直面している課題：数値発散（ブローアップ）による計算の破綻**
既存のSDBモデルをそのままプログラムに落とし込んで実行した際、計算途中で数値が無限大に発散し、シミュレーションがクラッシュする問題に直面しています。

**現在のアプローチ：飽和効果の導入と検証サイクル**
この破綻を根本的に防ぐため、異常な数値の伸びを抑え込む「飽和効果」をモデルに追加実装しました。現段階では学術的な完全解決には至っていませんが、$u$（x軸方向）と $v$（y軸方向）のパラメータの組み合わせを自動化して大量に実行・出力し、破綻しない条件の境界線を探る検証サイクルを日々回し続けています。

### 現在のシミュレーション進捗（動画）
（※ここに、今出せる最もマシな状態、あるいは飽和効果を入れて挙動が変化した動画を置きます）
| 2D View (Heatmap) |
| :---: |
| <video src="https://github.com/user-attachments/assets/3e624deb-bbce-448b-a436-548c306e4057" width="100%" autoplay loop muted></video> |

<video src="https://github.com/user-attachments/assets/0f495119-ab2c-4d6a-8199-f485e7157a2b"　controls width="300"></video>
<video src="https://github.com/user-attachments/assets/3e624deb-bbce-448b-a436-548c306e4057" controls width="300"></video>
<video src="https://github.com/user-attachments/assets/a6e0bc53-8905-48c4-8982-69f74dbf5b76" controls width="300"></video>
