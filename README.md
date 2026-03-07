# Numerical Simulation of Pattern Formation

大学院の研究で実装した、反応拡散系（SDBモデル等）の数値シミュレーションプログラムです。
偏微分方程式を離散化し、C言語を用いてゼロから数値計算を行っています。

## 出力結果（Gnuplotによる可視化）
計算結果のデータをGnuplotで描画し、パターンの形成過程を動画化しています。
パラメータによって動的にパターンが変化していく様子を確認できます。

### 変数 u, v, w のシミュレーション推移
<video src="OriSdbc06k8_u.mp4" controls width="300"></video>
<video src="OriSdbc06k8_v.mp4" controls width="300"></video>
<video src="OriSdbc06k8_w.mp4" controls width="300"></video>

## 使用技術
- **言語:** C言語
- **可視化:** Gnuplot
- **工夫した点（計算の最適化と収束判定）:** 計算処理において、システムが定常状態に達した（パターンの変化が最小になった）ことを検知して自動的にループを終了する関数を実装しています。これにより、無駄な計算リソースの消費を防ぎ、シミュレーションを効率化する工夫をしています。
