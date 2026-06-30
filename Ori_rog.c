#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

#define N 17.72  // 空間サイズ X
#define M 17.72  // 空間サイズ Y
#define Nx 100   // 空間分割数 X
#define Ny 100   // 空間分割数 Y
#define Tc 10000 // 書き出し間隔

// 3変数分の配列を用意
double u[Nx+1][Ny+1], v[Nx+1][Ny+1], w[Nx+1][Ny+1];
double u_new[Nx+1][Ny+1], v_new[Nx+1][Ny+1], w_new[Nx+1][Ny+1];

// 収束判定
double EquilibriumCheck(void){
    int i,j;
    double difMax=0.0, e;
    for(i=0; i<=Nx; i++){
        for(j=0; j<=Ny; j++){
            e = (fabs(u_new[i][j] - u[i][j]) 
              + fabs(v_new[i][j] - v[i][j]) 
              + fabs(w_new[i][j] - w[i][j]))/3.0;
            if(e > difMax) difMax = e;
        }
    }
    return difMax;
}

int main(void){
    FILE *data;
    double delta_x, delta_y, delta_t;
    
    // モデル固有の定数
    double a = 1.0, b = 1.0;
    double c = 1.0, chi = 14.0, mu = 1.0;
    double f = 1.0, g = 1.0;
    double d = 1.0;
    
    // 【追加】飽和効果の上限パラメータ K
    double K = 3.0; 

    // 定常解（初期値の基準）
    int init_mode = 0;
    double us, vs, ws; 
    double c_cr = sqrt((mu*mu*mu)/(2.0*b));
    printf("Critical Value c_cr = %f, Current c = %f\n", c_cr, c);

    if (c <= c_cr) {
        us = c/mu;
        vs = c/mu;
        ws = (2.0*f*c)/(mu*g);
        printf("Mode: Single solution (Pre-bifurcation). us=%f\n", us);
    } else {
        double u_center, u_plus, u_minus;
        double root_term = sqrt( (c*c)/(mu*mu) - (mu)/(2.0*b) );

        u_center = c/mu;
        u_plus   = u_center + root_term; 
        u_minus  = u_center - root_term;

        switch (init_mode) {
            case 0: us = u_center; vs = u_center; ws = (2.0*f*c)/(mu*g); break;
            case 1: us = u_plus;   vs = u_minus;  ws = (2.0*f*c)/(mu*g); break;
            case 2: us = u_minus;  vs = u_plus;   ws = (2.0*f*c)/(mu*g); break;
            default: printf("Error: Invalid init_mode\n"); return 1;
        }
        printf("Mode: Bifurcated Solution (%d). us=%f, vs=%f, ws=%f\n", init_mode, us, vs, ws);
    }
    
    double difMax;
    double eps = 1e-10; 
    int i, j, m;
    int iu, id, ju, jd; 
    int timeskip = Tc;
    char result[256];
    char output_dir[] = "./Ori_rogc10chi14k3"; 

    // 空間・時間刻み
    delta_x = N / (double)Nx;
    delta_y = M / (double)Ny;
    delta_t = 1e-5; 
    
    // 初期値の設定
    srand((unsigned)time(NULL));
    for(i=0; i<=Nx; i++){
        for(j=0; j<=Ny; j++){
            u[i][j] = us + ((rand()*20.0/(1.0+RAND_MAX))-5.0)/100.0;
            v[i][j] = vs + ((rand()*20.0/(1.0+RAND_MAX))-5.0)/100.0;
            w[i][j] = ws + ((rand()*20.0/(1.0+RAND_MAX))-5.0)/100.0;
        }
    }

    // 初期のデータ保存（事前に ./Ori_rogc0k00 フォルダを作成しておくこと）
    sprintf(result, "%s/data%d.txt", output_dir, 0);
    data = fopen(result, "w");
    if(data == NULL){
        printf("エラー: ディレクトリ '%s' が存在しないか、書き込めません。\n", output_dir);
        return 1;
    }
    for(i=0; i<=Nx; i++){
        for(j=0; j<=Ny; j++){
            fprintf(data, "%f %f %.10f %.10f %.10f\n", i*delta_x, j*delta_y, u[i][j], v[i][j], w[i][j]);
        }
        fprintf(data, "\n");
    }
    fclose(data);

    m = 0;
    // 時間発展ループ
    do {
        m++;
        for(i=0; i<=Nx; i++){
            // 境界条件（ノイマン条件：反射）
            iu = i+1; if(iu == Nx+1) iu = Nx-1;
            id = i-1; if(id == -1)   id = 1;

            for(j=0; j<=Ny; j++){
                ju = j+1; if(ju == Ny+1) ju = Ny-1;
                jd = j-1; if(jd == -1)   jd = 1;

                // -------------------------------------------------------------
                // 【説明ポイント】純粋な空間の2階微分（ラプラシアン）を計算
                // -------------------------------------------------------------
                double d2u_dx2 = (u[iu][j] - 2.0*u[i][j] + u[id][j]) / (delta_x * delta_x);
                double d2u_dy2 = (u[i][ju] - 2.0*u[i][j] + u[i][jd]) / (delta_y * delta_y);
                
                double d2v_dx2 = (v[iu][j] - 2.0*v[i][j] + v[id][j]) / (delta_x * delta_x);
                double d2v_dy2 = (v[i][ju] - 2.0*v[i][j] + v[i][jd]) / (delta_y * delta_y);
                
                double d2w_dx2 = (w[iu][j] - 2.0*w[i][j] + w[id][j]) / (delta_x * delta_x);
                double d2w_dy2 = (w[i][ju] - 2.0*w[i][j] + w[i][jd]) / (delta_y * delta_y);

                double lap_u = d2u_dx2 + d2u_dy2;
                double lap_v = d2v_dx2 + d2v_dy2;
                double lap_w = d2w_dx2 + d2w_dy2;

                // -------------------------------------------------------------
                // 【説明ポイント】時間発展の計算（支配方程式との完全な対応）
                // -------------------------------------------------------------
                
                // 第1式 (uの更新): 飽和効果 (1 - u/K) を追加
                u_new[i][j] = u[i][j] + delta_t * (
                    a * lap_u 
                    - chi * (1.0 - u[i][j] / K) * u[i][j] * d2w_dx2 
                    + b * u[i][j] * v[i][j] * (u[i][j] - v[i][j]) 
                    + c 
                    - mu * u[i][j]
                );

                // 第2式 (vの更新): 飽和効果 (1 - v/K) を追加
                v_new[i][j] = v[i][j] + delta_t * (
                    a * lap_v 
                    - chi * (1.0 - v[i][j] / K) * v[i][j] * d2w_dy2 
                    + b * u[i][j] * v[i][j] * (v[i][j] - u[i][j]) 
                    + c 
                    - mu * v[i][j]
                );

                // 第3式 (wの更新)
                w_new[i][j] = w[i][j] + delta_t * (
                    d * (u[i][j] + v[i][j]) * lap_w 
                    + f * u[i][j] 
                    + f * v[i][j] 
                    - g * w[i][j]
                );
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

        if(m == 1e8) break; // 無限ループ防止

    } while(difMax > eps || m % timeskip != 0);

    printf("Simulation Completed.\n");
    return 0;
}