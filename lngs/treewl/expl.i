#import stdt
#import STree.Utils
#import STree.Monad

#grammar "intern.l"
#runer   "intern.r"
#ccall   ""


data_one {
	i = 0;
	while (i < 10) {
		print i;
		i = i + 1;
	}
}

data_two {
	let
		iter 
}

