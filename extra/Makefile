free = dtls.pdf p2pnat.pdf
ieee = ELA-VPN.pdf
WGET = wget

all allextra: free ieee

free: $(free)

ieee: $(ieee)

clean cleanextra:
	rm -f $(free) $(ieee)

dtls.pdf:
	$(WGET) -O $@ "http://crypto.stanford.edu/~nagendra/papers/dtls.pdf"

p2pnat.pdf:
	$(WGET) -O $@ "http://www.bford.info/pub/net/p2pnat.pdf"

ELA-VPN.pdf:
	$(WGET) -O $@ "http://ieeexplore.ieee.org/iel5/9522/30170/01386101.pdf?tp=&isnumber=30170&arnumber=1386101"
