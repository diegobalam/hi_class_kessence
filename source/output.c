/** @file output.c Documented output module
 * Julien Lesgourgues, 18.04.2010    
 *
 * This module writes the output in files.
 */

#include "output.h"

int output_init(
		struct background * pba_input,
		struct perturbs * ppt_input,
		struct transfers * ptr_input,
		struct spectra * psp_input,
		struct output * pop
		) {

  FILE * * out;
  FILE * outbis;
  int index_mode,index_ic,index_ct,l,index_k;
  double * cl_output;

  if (pop->output_verbose > 0)
    printf("Writing in output files \n");

  if (ptr_input->tt_size >0) {

    for (index_mode = 0; index_mode < ppt_input->md_size; index_mode++) {

      out = malloc(ppt_input->ic_size[index_mode]*sizeof(FILE *));
      if (out == NULL) {
	sprintf(pop->error_message,"%s(L:%d) : Could not allocate out",__func__,__LINE__);
	return _FAILURE_;
      }

      cl_output= malloc(ppt_input->ic_size[index_mode]*psp_input->ct_size*sizeof(double));

      if (cl_output == NULL) {
	sprintf(pop->error_message,"%s(L:%d) : Could not allocate cl_output",__func__,__LINE__);
	return _FAILURE_;
      }

      for (index_ic = 0; index_ic < ppt_input->ic_size[index_mode]; index_ic++) {

	if ((ppt_input->has_scalars) && (index_mode == ppt_input->index_md_scalars)) {
	  if ((ppt_input->has_ad) && (index_ic == ppt_input->index_ic_ad)) {
	    out[index_ic]=fopen(pop->cls_ad,"w");
	    if (out[index_ic] == NULL) {
	      sprintf(pop->error_message,"%s(L:%d) : Could not open out[index_ic]",__func__,__LINE__);
	      return _FAILURE_;
	    }
	  }
	}
      }

      for (l = 2; l <= psp_input->l[index_mode][psp_input->l_size[index_mode]-1]; l++) {  

	if (spectra_cl_at_l((double)l,index_mode,cl_output) == _FAILURE_) {
	  sprintf(pop->error_message,"%s(L:%d) : error in spectra_cl_at_l()\n=>%s",__func__,__LINE__,psp_input->error_message);
	  return _FAILURE_;
	}

	for (index_ic = 0; index_ic < ppt_input->ic_size[index_mode]; index_ic++) {

	  fprintf(out[index_ic],"%d",l);
	  for (index_ct=0; index_ct < psp_input->ct_size; index_ct++) {
	    fprintf(out[index_ic]," ",l);
	    fprintf(out[index_ic],"%e",l*(l+1)*cl_output[index_ic * psp_input->ct_size + index_ct]);
	  }
	  fprintf(out[index_ic],"\n");	
	  
	}

      }

      for (index_ic = 0; index_ic < ppt_input->ic_size[index_mode]; index_ic++) {

	fclose(out[index_ic]);

      }

      free(out);
      free(cl_output);

    }
  }

  if (ppt_input->has_pk_matter == _TRUE_) {

    outbis=fopen(pop->pk,"w");

/*     fprintf(outbis,"# Number of redshifts z:\n"); */
/*     fprintf(outbis,"%d\n",1); */
    fprintf(outbis,"# Number of wavenumbers k:\n");
    fprintf(outbis,"%d\n",psp_input->k_size);
    fprintf(outbis,"# k (h/Mpc)  P (Mpc/h)^3:\n");

    index_mode=ppt_input->index_md_scalars;

    for (index_k=0; index_k<psp_input->k_size; index_k++) {

      fprintf(outbis,"%e",psp_input->k[index_k]/pba_input->h);

      for (index_ic = 0; index_ic < ppt_input->ic_size[index_mode]; index_ic++) {
	fprintf(outbis," %e",
		pow(pba_input->h,3)*psp_input->pk[index_ic * psp_input->k_size + index_k]);
      }
      fprintf(outbis,"\n");
    }
    fclose(outbis);

  }

  return _SUCCESS_;

}