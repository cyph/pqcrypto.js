#include "fft.h"


void fft_sub_1(modp_context_t* ctx,int x[], int N, int tau)
{
    int i, beta;
    int h = N / 2;
    int y[LIMA_MAX_BD / 4];
    int z[LIMA_MAX_BD / 4];

    if (N == 1) return;

    for (i = 0; i<h; i++)
    {
        y[i] = x[i * 2];
        z[i] = x[i * 2 + 1];
    }
    beta = mul_mod_q(ctx,tau, tau);
    fft_sub_1(ctx, y, h, beta);
    fft_sub_1(ctx, z, h, beta);
    int omega = tau;

    for (i = 0; i<h; i++)
    {
        int s = mul_mod_q(ctx, omega, z[i]);
        int t = y[i];
        x[i] = add_mod_q(ctx, t, s);
        x[i + h] = sub_mod_q(ctx, t, s);
        omega = mul_mod_q(ctx, omega, beta);
    }
}

void fft_sub_2(modp_context_t* ctx, int x[], int N, int tau)
{
    int i, beta;
    int h = N / 2;
    int y[LIMA_MAX_BD / 2];
    int z[LIMA_MAX_BD / 2];

    if (N == 1) return;

    for (i = 0; i<h; i++)
    {
        y[i] = x[i * 2];
        z[i] = x[i * 2 + 1];
    }
    beta = mul_mod_q(ctx,tau, tau);
    fft_sub_2(ctx, y, h, beta);
    fft_sub_2(ctx, z, h, beta);
    int omega = to_mod_q(ctx,1);

    for (i = 0; i<h; i++)
    {
        int s = mul_mod_q(ctx, omega, z[i]);
        int t = y[i];
        x[i] = add_mod_q(ctx, t, s);
        x[i + h] = sub_mod_q(ctx, t, s);
        omega = mul_mod_q(ctx, omega, tau);
    }
}



/* Subroutine used in the LIMA-sp variant  to compute  forward and backward 
 * length-N FFT transformations  of a
 * The setting r=0 implements forward direction and r=1 backward direction
 * Non-power of two FFT in LIMA-sp is implemented using
 * Bluestein's trick
 */
void bfft(lima_params_t* params, const int a[], int r, int b[])
{
    int N = params->N;
    int p = N + 1;
    int e = params->e;
    int two_pow_e = 1 << e;
    int x[LIMA_MAX_BD];
    int i;
   
    for (i = 0; i<p; i++) x[i] = mul_mod_q(params->ctx, params->fft_data->powers[r][i], a[i]);
    for (i = p; i<two_pow_e; i++) x[i] = 0;

    fft_sub_2(params->ctx, x, two_pow_e, params->fft_data->beta0);

    for (i = 0; i<two_pow_e; i++) x[i] = mul_mod_q(params->ctx,x[i], params->fft_data->bd[r][i]);

    fft_sub_2(params->ctx,x, two_pow_e, params->fft_data->beta1);

    for (i = 0; i<p; i++) b[i] = mul_mod_q(params->ctx, x[i + p - 1], params->fft_data->powersi[r][i]);
}


void fft(lima_params_t* params, const int f[], int x[])
{
    int i;
    int N = params->N;

    if (params->mode == LIMA_2P)
    {
        for (i = 0; i<N; i++) x[i] = f[i];
        fft_sub_1(params->ctx, x, N, params->fft_data->alpha0);
    }
    else // LIMA_SP
    {
        int p = N + 1;
        int f_tag[LIMA_MAX_P];
        int y[LIMA_MAX_P];

        for (i = 0; i<p - 1; i++) f_tag[i] = f[i];
        f_tag[p - 1] = 0;

        bfft(params, f_tag, 0, y);
        for (i = 0; i<p - 1; i++) x[i] = y[i + 1];
    }
}

void fft_inv(lima_params_t* params, const int x[], int f[])
{
    int i;
    int N = params->N;
    if (params->mode == LIMA_2P)
    {
        int gamma = mul_mod_q(params->ctx,params->fft_data->alpha1, params->fft_data->alpha1);
        int delta = params->fft_data->beta0;

        for (i = 0; i<N; i++) f[i] = x[i];
        fft_sub_2(params->ctx,f, N, gamma);

        for (i = 0; i<N; i++)
        {
            f[i] = mul_mod_q(params->ctx,f[i], delta);
            delta = mul_mod_q(params->ctx, delta, params->fft_data->alpha1);
        }
    }
    else // LIMA_SP
    {
        int p = N + 1;
        int y[LIMA_MAX_P];
        int f_tag[LIMA_MAX_P];

        y[0] = 0;
        for (i = 0; i<p - 1; i++) y[i + 1] = x[i];

        bfft(params, y, 1, f_tag);
        for (i = 0; i<p - 1; i++) 
          { f[i] = sub_mod_q(params->ctx,f_tag[i],f_tag[p - 1]); }
    }
}


// Naive inverse mod q on standard representation integers
int inv_mod_q(int q, int x)
{
    int q0 = q;
    int t, d;
    int x0 = 0, x1 = 1;

    while (x > 1)
    {
        d = x / q;
        t = q;
        q = x % q;
        x = t;
        t = x0;
        x0 = x1 - d * x0;
        x1 = t;
    }
    if (x1 < 0) x1 += q0;
    return x1; 
}


void precompute_fft_data(lima_params_t* params)
{
    // Convert human readable data into internal representation if needed
    params->fft_data->alpha0 = to_mod_q(params->ctx, params->alpha0);
    params->fft_data->alpha1 = to_mod_q(params->ctx, params->alpha1);
    params->fft_data->beta0  = to_mod_q(params->ctx, params->beta0);
    params->fft_data->beta1  = to_mod_q(params->ctx, params->beta1);

    // Set up Bluestein data
    if (params->mode == LIMA_SP)  
    { 
        int e = params->e;
        int p = params->N + 1;
        int two_pow_e = 1<<e;
        int s, b, r, i;

        int* powers[2] =  { params->fft_data->powers[0],  params->fft_data->powers[1]  };
        int* powersi[2] = { params->fft_data->powersi[0], params->fft_data->powersi[1] };
        int* bd[2] =      { params->fft_data->bd[0],      params->fft_data->bd[1]      };

        powersi[0][0] = inv_mod_q(params->ctx->q, two_pow_e);
        powersi[0][0] = to_mod_q(params->ctx, powersi[0][0]); 
        powersi[1][0] = inv_mod_q(params->ctx->q, (p*two_pow_e)%params->ctx->q);
        powersi[1][0] = to_mod_q(params->ctx, powersi[1][0]);

        for (r = 0; r<2; r++)
        {
            int alpha_r = (r == 0) ? params->fft_data->alpha0 : params->fft_data->alpha1;
            int alpha_1_minus_r = (r == 0) ? params->fft_data->alpha1 : params->fft_data->alpha0;
            
            powers[r][0] = to_mod_q(params->ctx,1);
            bd[r][p - 1] = to_mod_q(params->ctx,1);
            for (i = 1; i <= p - 1; i++)
            {
                s = (i*i)%(2*p);
                powers[r][i] = exp_mod_q(params->ctx, alpha_r, s);
                powersi[r][i] = mul_mod_q(params->ctx, powers[r][i], powersi[r][0]);
                b = exp_mod_q(params->ctx, alpha_1_minus_r, s);
                bd[r][p - 1 + i] = b;
                bd[r][p - 1 - i] = b;
            }

            for (i = 2 * p - 1; i<two_pow_e; i++) bd[r][i] = 0;
            fft_sub_2(params->ctx, bd[r], two_pow_e, params->fft_data->beta0);
        }
    }
}
