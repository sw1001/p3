#define N 50 // N > index, which is the total number of literals
#define D 80 // D >= dim1_size
#define S 900// S >= size

#define BASE 4294967296UL
//! Represents the state of a particular generator
typedef struct{ uint x; uint c; } mwc64x_state_t;

enum{ MWC64X_A = 4294883355U };
enum{ MWC64X_M = 18446383549859758079UL };

void MWC64X_Step(mwc64x_state_t *s)
{
	uint X=s->x, C=s->c;
	
	uint Xn=MWC64X_A*X+C;
	uint carry=(uint)(Xn<C);				// The (Xn<C) will be zero or one for scalar
	uint Cn=mad_hi(MWC64X_A,X,carry);  
	
	s->x=Xn;
	s->c=Cn;
}

//! Return a 32-bit integer in the range [0..2^32)
uint MWC64X_NextUint(mwc64x_state_t *s)
{
	uint res=s->x ^ s->c;
	MWC64X_Step(s);
	return res;
}



__kernel void setInfluence(const int literals, const int size, const int dim1_size, __global int* lambdas, __global float* lambdap, __global int* dim2_size, __global float* influence){   
	int flag=get_global_id(0);
	int sum=0;
	int count=10000;
	int assignment[N];
	int lambdas_t[S];
	int lambdas_f[S];
	int dim2_size_t[D];
	int dim2_size_f[D];
	int index_t=0;
	int index_f=0;
	int index=0;
	
	
    //try to get newlambda like original version does
    if(flag < literals){
		mwc64x_state_t rng;
		//new lambda
		for(int m=0; m<dim1_size; m++){
			dim2_size_t[m]=dim2_size[m];
			dim2_size_f[m]=0;
			int flag_in_mono=0;
			for(int n=0; n<dim2_size[m]; n++){
				if(lambdas[index+n]!=flag){
					lambdas_t[index_t]=lambdas[index+n];
					index_t ++;					
				}
				else{
					dim2_size_t[m] --;
					flag_in_mono=1;				
				}
				lambdas_f[index_f+n]=lambdas[index+n];
								
			}
			if(flag_in_mono==0){
				dim2_size_f[m]=dim2_size[m];
				index_f+=dim2_size[m];
			}
			index+=dim2_size[m];
			
		}
		
		
		
		
		for(int i=0; i<count; i++){
			for(int j=0; j<size; j++){
			    float rand=MWC64X_NextUint(&rng)*1.0/BASE;
				if(lambdap[j]<rand)
					assignment[lambdas[j]]=0;
				else
					assignment[lambdas[j]]=1;				
			}
		    //the true case
		    int valuet=0;
		    index=0;
		    for(int m=0; m<dim1_size; m++){
		    	int valueMono=1;
		    	for(int n=0; n<dim2_size_t[m]; n++){
		        	if(assignment[lambdas_t[index+n]]==0){
		        		valueMono=0;
		        		index+=dim2_size_t[m];
		        		break;
		        	}
		    	}
		    	if(valueMono==1 && dim2_size_t[m]!=0){
		        	valuet=1;
		        	break;
		    	}
		    }        
		    //the false case
		    int valuef=0;
		    index=0;
		    for(int m=0; m<dim1_size; m++){
		    	int valueMono=1;
		    	for(int n=0; n<dim2_size_f[m]; n++){
		        	if(assignment[lambdas_f[index+n]]==0){
		        		valueMono=0;
		        		index+=dim2_size_f[m];
		        		break;
		        	}
		    	}
		    	if(valueMono==1 && dim2_size_f[m]!=0){
		        	valuef=1;
		        	break;
		    	}
		    }
		    sum += valuet-valuef;            
		}
		influence[flag] = 1.0*sum/count;
		printf("%d:", flag);
		printf("%f  ", influence[flag]);
	}
}  


