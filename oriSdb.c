#include<stdio.h>
#include<math.h>
#include<stdlib.h>

#include<time.h>
#include<sys/stat.h> // ディレクトリ作成用（環境による）

// パラメータ定義（※モデルに合わせて変更が必要）
#define N 17.72  // 空間サイズ X
#define M 17.72  // 空間サイズ Y
#define Nx 100   // 空間分割数 X
#define Ny 100   // 空間分割数 Y
#define Tc 10000 // 書き出し間隔

// 3変数分の配列を用意
double u[Nx+1][Ny+1], v[Nx+1][Ny+1], w[Nx+1][Ny+1];
double u_new[Nx+1][Ny+1], v_new[Nx+1][Ny+1], w_new[Nx+1][Ny+1];

// 収束判定（3変数すべてが変化しなくなったら終了）
double EquilibriumCheck(void){
    int i,j;
    double difMax=0.0, e;
    for(i=0; i<=Nx; i++){
        for(j=0; j<=Ny; j++){
            // u, v, w 全ての差分をチェック
            e = (fabs(u_new[i][j] - u[i][j]) 
              + fabs(v_new[i][j] - v[i][j]) 
              + fabs(w_new[i][j] - w[i][j]))/3.0;
            // 平均を取るか合計を取るかは判定の厳しさによる（ここでは合計で簡易化）
            if(e > difMax) difMax = e;
        }
    }
    return difMax;
}

int main(void){
    FILE *data;
    double delta_x, delta_y, delta_t;
    double alpha_x, alpha_y; // 拡散計算用係数
    
    // モデル固有の定数
    double a=1.0,b=1.0;
    double c=0.6,chi=8.0,mu=1.0;
    double f=1.0,g=1.0;
    double rho=1.0;
    double d=1.0;

    // 定常解（初期値の基準）
    int init_mode=0;
    double us ,vs, ws ; 
    double c_cr;
    c_cr=sqrt((mu*mu*mu)/(2*b));
    printf("Critical Value c_cr = %f, Current c = %f\n", c_cr, c);

    // 条件判定を修正: c <= c_cr なら解は1つ、それより大きければ分岐
    if (c <= c_cr)
    {
        // 【ケース1: 単一解 (cが小さい時)】
        // ※ルートの中がマイナスになるので、こちらのルートでは3つの解は計算できない
        us = c/mu;
        vs = c/mu;
        ws = (2.0*f*c)/(mu*g);

        printf("Mode: Single solution (Pre-bifurcation). us=%f\n", us);
    }
    else
    {
        // 【ケース2: 解が3つある場合 (cが大きい時)】
        double u_center, u_plus, u_minus;
        
        // 共通部分を先に計算（ルートの中身）
        // pow(x, 2.0) か x*x を使用
        double root_term = sqrt( (c*c)/(mu*mu) - (mu)/(2.0*b) );

        u_center = c/mu;
        u_plus   = u_center + root_term; 
        u_minus  = u_center - root_term;

        switch (init_mode) {
            case 0: // 中央（不安定解）
                us = u_center;
                vs = u_center; 
                ws = (2.0*f*c)/(mu*g);
                break;
            case 1: // プラス側の安定解
                us = u_plus;
                vs = u_minus; // 対称的に配置
                ws = (2.0*f*c)/(mu*g);
                break;
            case 2: // マイナス側の安定解
                us = u_minus;
                vs = u_plus;  // 対称的に配置
                ws = (2.0*f*c)/(mu*g);
                break;
            default:
                printf("Error: Invalid init_mode\n");
                return 1;
        }
        printf("Mode: Bifurcated Solution (%d). us=%f, vs=%f,ws=%f\n", init_mode, us, vs, ws);
    }
    
    
    double difMax;
    double eps = 1e-10; // 収束判定基準
    int i, j, m;
    int iu, id, ju, jd; // 境界計算用インデックス
    int timeskip = Tc;
    char result[256];
    char output_dir[] = "./OriSdbc06k8"; // 保存先ディレクトリ

    // ディザスタライゼーション（空間刻み）
    delta_x = N / (double)Nx;
    delta_y = M / (double)Ny;
    delta_t = 1e-5; // ※拡散係数との兼ね合いで発散しないよう注意が必要（CFL条件）
    
    // 単純な拡散方程式の場合の係数（モデルによる）
    alpha_x = delta_t / (delta_x * delta_x);
    alpha_y = delta_t / (delta_y * delta_y);
    
    // 初期値の設定 (u_0, v_0, w_0)
    srand((unsigned)time(NULL));
    for(i=0; i<=Nx; i++){
        for(j=0; j<=Ny; j++){
            // ※とりあえず全てにランダムノイズを入れています
            u[i][j] = us + ((rand()*20.0/(1.0+RAND_MAX))-5.0)/100.0;
            v[i][j] = vs + ((rand()*20.0/(1.0+RAND_MAX))-5.0)/100.0;
            w[i][j] = ws + ((rand()*20.0/(1.0+RAND_MAX))-5.0)/100.0;
        }
    }

    // 初期状態の保存
    // フォルダがないとエラーになるため、事前に作成するか確認してください
    sprintf(result, "%s/data%d.txt", output_dir, 0);
    data = fopen(result, "w");
    if(data == NULL){
        printf("エラー: ディレクトリ '%s' が存在しないか、書き込めません。\n", output_dir);
        return 1;
    }
    for(i=0; i<=Nx; i++){
        for(j=0; j<=Ny; j++){
            // 3変数を出力
            fprintf(data, "%f %f %.10f %.10f %.10f\n", i*delta_x, j*delta_y, u[i][j], v[i][j], w[i][j]);
        }
        fprintf(data, "\n");
    }
    fclose(data);

    m = 0;
    // 時間発展ループ
    do{
        m++;
        for(i=0; i<=Nx; i++){
            // 境界条件（ノイマン条件：反射）
            iu = i+1; if(iu == Nx+1) iu = Nx-1;
            id = i-1; if(id == -1)   id = 1;

            for(j=0; j<=Ny; j++){
                ju = j+1; if(ju == Ny+1) ju = Ny-1;
                jd = j-1; if(jd == -1)   jd = 1;

                
                
                double lap_u = (u[iu][j] - 2.0*u[i][j] + u[id][j])*alpha_x + (u[i][ju] - 2.0*u[i][j] + u[i][jd])*alpha_y;
                double lap_v = (v[iu][j] - 2.0*v[i][j] + v[id][j])*alpha_x + (v[i][ju] - 2.0*v[i][j] + v[i][jd])*alpha_y;
                double lap_w = (w[iu][j] - 2.0*w[i][j] + w[id][j])*alpha_x + (w[i][ju] - 2.0*w[i][j] + w[i][jd])*alpha_y;


                // 次の時刻の値を計算
                u_new[i][j] = u[i][j] + a*lap_u-chi*u[i][j]*((w[iu][j] - 2.0*w[i][j] + w[id][j])*alpha_x)+delta_t*(b*u[i][j]*v[i][j]*(u[i][j]-v[i][j])+c-rho*u[i][j]);
                v_new[i][j] = v[i][j] + a*lap_v-chi*u[i][j]*((w[i][ju] - 2.0*w[i][j] + w[i][jd])*alpha_y)+delta_t*(b*u[i][j]*v[i][j]*(v[i][j]-u[i][j])+c-rho*u[i][j]);
                w_new[i][j] = w[i][j] + d*(u[i][j]+v[i][j])*lap_w+delta_t*(f*u[i][j]+f*v[i][j]-g*w[i][j]);
                
                
            }
        }

        // 収束判定
        difMax = EquilibriumCheck();

        // 途中経過の出力
        if(m % timeskip == 0){
            sprintf(result, "%s/data%d.txt", output_dir, m);
            data = fopen(result, "w");
            for(i=0; i<=Nx; i++){
                for(j=0; j<=Ny; j++){
                    fprintf(data, "%f %f %.10f %.10f %.10f\n", i*delta_x, j*delta_y, u_new[i][j], v_new[i][j], w_new[i][j]);
                }
                fprintf(data, "\n");
            }
            fclose(data);
            printf("Step: %d, DifMax: %.15f\n", m, difMax);
        }

        // 配列の更新
        for(i=0; i<=Nx; i++){
            for(j=0; j<=Ny; j++){
                u[i][j] = u_new[i][j];
                v[i][j] = v_new[i][j];
                w[i][j] = w_new[i][j];
            }
        }

        // 無限ループ防止（必要に応じて調整）
        if(m == 1e8) break;

    } while(difMax > eps || m % timeskip != 0);

    printf("Simulation Completed.\n");
    return 0;
}