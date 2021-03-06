#include "pch.h"
#include "eccrypto.h"
#include "ec2n.h"
#include "ecp.h"
#include "nbtheory.h"

NAMESPACE_BEGIN(CryptoPP)

// VC60 workaround: complains when these functions are put into an anonymous namespace
static Integer ConvertToInteger(const PolynomialMod2 &x)
{
	unsigned int l = x.ByteCount();
	SecByteBlock temp(l);
	x.Encode(temp, l);
	return Integer(temp, l);
}

static inline Integer ConvertToInteger(const Integer &x)
{
	return x;
}

bool static CheckMOVCondition(const Integer &q, const Integer &r)
{
	Integer t=1;
	unsigned int n=q.BitCount(), m=r.BitCount();

	for (unsigned int i=n; DiscreteLogWorkFactor(i)<m/2; i+=n)
	{
		t = (t*q)%r;
		if (t == 1)
			return false;
	}
	return true;
}

// ******************************************************************

template <class EC> ECPublicKey<EC>::ECPublicKey(const EC &E, const Point &P, const Point &Q, const Integer &orderP)
	: ec(E), P(P), Q(Q), n(orderP), 
	  Ppc(E, P, ExponentBitLength(), 1), Qpc(E, Q, ExponentBitLength(), 1)
{
	l = ec.GetField().MaxElementByteLength();
	f = n.ByteCount();
}

template <class EC> ECPublicKey<EC>::~ECPublicKey()
{
}

template <class EC> void ECPublicKey<EC>::Precompute(unsigned int precomputationStorage)
{
	Ppc.Precompute(P, ExponentBitLength(), precomputationStorage);
	Qpc.Precompute(Q, ExponentBitLength(), precomputationStorage);
}

template <class EC> void ECPublicKey<EC>::LoadPrecomputation(BufferedTransformation &bt)
{
	Ppc.Load(bt);
	Qpc.Load(bt);
}

template <class EC> void ECPublicKey<EC>::SavePrecomputation(BufferedTransformation &bt) const
{
	Ppc.Save(bt);
	Qpc.Save(bt);
}

template <class EC>
Integer ECPublicKey<EC>::EncodeDigest(const byte *digest, unsigned int digestLen) const
{
	Integer h;
	if (digestLen*8 < n.BitCount())
		h.Decode(digest, digestLen);
	else
	{
		h.Decode(digest, n.ByteCount());
		h >>= n.ByteCount()*8 - n.BitCount() + 1;
	}
	assert(h < n);
	return h;
}

template <class EC> void ECPublicKey<EC>::Encrypt(RandomNumberGenerator &rng, const byte *plainText, unsigned int plainTextLength, byte *cipherText)
{
	assert (plainTextLength <= MaxPlainTextLength());

	Integer k(rng, 2, n-2, Integer::ANY);
	Point kP = Ppc.Multiply(k);
	Point kQ = Qpc.Multiply(k);

	cipherText[0] = 0;
	kP.x.Encode(cipherText+1, l);
	kP.y.Encode(cipherText+l+1, l);
	kQ.x.Encode(cipherText+2*l+1, l);

	SecByteBlock paddedBlock(l-1);
	// pad with non-zero random bytes
	for (unsigned i = 0; i < l-2-plainTextLength; i++)
		while ((paddedBlock[i] = rng.GetByte()) == 0);
	paddedBlock[l-2-plainTextLength] = 0;
	memcpy(paddedBlock+l-1-plainTextLength, plainText, plainTextLength);
	xorbuf(cipherText+2*l+2, paddedBlock, l-1);
}

template <class EC> bool ECPublicKey<EC>::VerifyDigest(const byte *digest, unsigned int digestLen, const byte *signature) const
{
	assert (digestLen <= MaxDigestLength());

	Integer e = EncodeDigest(digest, digestLen);
	Integer r(signature, f);
	Integer s(signature+f, f);

	// convert kP.x into a Integer
	Integer x = ConvertToInteger(Ppc.CascadeMultiply(s, Qpc, r).x);
	return !!r && r == (x+e)%n;
}

// ******************************************************************

template <class EC> ECPrivateKey<EC>::~ECPrivateKey()
{
}

template <class EC> void ECPrivateKey<EC>::Randomize(RandomNumberGenerator &rng)
{
	d.Randomize(rng, 2, n-2, Integer::ANY);
	Q = Ppc.Multiply(d);
	Qpc.Precompute(Q, ExponentBitLength(), 1);
}

template <class EC> unsigned int ECPrivateKey<EC>::Decrypt(const byte *cipherText, byte *plainText)
{
	if (cipherText[0]!=0)	// TODO: no support for point compression yet
		return 0;

	FieldElement kPx(cipherText+1, l);
	FieldElement kPy(cipherText+l+1, l);
	Point kP(kPx, kPy);
	Point kQ(ec.Multiply(d, kP));

	SecByteBlock paddedBlock(l-1);
	kQ.x.Encode(paddedBlock, l-1);
	xorbuf(paddedBlock, cipherText+2*l+2, l-1);

	unsigned i;
	// remove padding
	for (i=0; i<l-1; i++)
		if (paddedBlock[i] == 0)			// end of padding reached
		{
			i++;
			break;
		}

	memcpy(plainText, paddedBlock+i, l-1-i);
	return l-1-i;
}

template <class EC> void ECPrivateKey<EC>::SignDigest(RandomNumberGenerator &rng, const byte *digest, unsigned int digestLen, byte *signature) const
{
	Integer r, s;
	Integer e = EncodeDigest(digest, digestLen);

	do
	{
		Integer k(rng, 2, n-2, Integer::ANY);
		// convert kP.x into an Integer
		Integer x = ConvertToInteger(Ppc.Multiply(k).x);
		r = (x+e)%n;
		s = (k-d*r)%n;
	} while (!r);

	r.Encode(signature, f);
	s.Encode(signature+f, f);
}

// ******************************************************************

template <class EC>
ECDHC<EC>::ECDHC(const EC &ec, const Point &G, const Integer &r, const Integer &k)
	: ec(ec), G(G), r(r), k(k), Gpc(ec, G, r.BitCount(), 1)
{
}

template <class EC>
ECDHC<EC>::~ECDHC()
{
}

template <class EC>
void ECDHC<EC>::Precompute(unsigned int precomputationStorage)
{
	Gpc.Precompute(G, r.BitCount(), precomputationStorage);
}

template <class EC>
void ECDHC<EC>::LoadPrecomputation(BufferedTransformation &bt)
{
	Gpc.Load(bt);
}

template <class EC>
void ECDHC<EC>::SavePrecomputation(BufferedTransformation &bt) const
{
	Gpc.Save(bt);
}

template <class EC>
bool ECDHC<EC>::ValidateDomainParameters(RandomNumberGenerator &rng) const
{
	Integer q = ec.FieldSize(), qSqrt = q.SquareRoot();

	return ec.ValidateParameters(rng) && r!=q && r>4*qSqrt && VerifyPrime(rng, r)
		&& ec.VerifyPoint(G) && !G.identity && ec.Multiply(r, G).identity
		&& k==(q+2*qSqrt+1)/r && CheckMOVCondition(q, r);
}

template <class EC>
void ECDHC<EC>::GenerateKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const
{
	Integer x(rng, 1, r-1);
	Point Q = Gpc.Multiply(x);
	x.Encode(privateKey, PrivateKeyLength());
	ec.EncodePoint(publicKey, Q);
}

template <class EC>
bool ECDHC<EC>::Agree(byte *agreedValue, const byte *privateKey, const byte *otherPublicKey, bool validateOtherPublicKey) const
{
	Point W(ec.DecodePoint(otherPublicKey));
	if (validateOtherPublicKey && !ec.VerifyPoint(W))
		return false;

	Integer s(privateKey, PrivateKeyLength());
	Point Q = ec.Multiply(k*s, W);
	if (Q.identity)
		return false;
	Q.x.Encode(agreedValue, AgreedValueLength());
	return true;
}

// ******************************************************************

template <class EC>
ECMQVC<EC>::ECMQVC(const EC &ec, const Point &G, const Integer &r, const Integer &k)
	: ec(ec), G(G), r(r), k(k), Gpc(ec, G, r.BitCount(), 1)
{
}

template <class EC>
ECMQVC<EC>::~ECMQVC()
{
}

template <class EC>
void ECMQVC<EC>::Precompute(unsigned int precomputationStorage)
{
	Gpc.Precompute(G, r.BitCount(), precomputationStorage);
}

template <class EC>
void ECMQVC<EC>::LoadPrecomputation(BufferedTransformation &bt)
{
	Gpc.Load(bt);
}

template <class EC>
void ECMQVC<EC>::SavePrecomputation(BufferedTransformation &bt) const
{
	Gpc.Save(bt);
}

template <class EC>
bool ECMQVC<EC>::ValidateDomainParameters(RandomNumberGenerator &rng) const
{
	Integer q = ec.FieldSize(), qSqrt = q.SquareRoot();

	return ec.ValidateParameters(rng) && r!=q && r>4*qSqrt && VerifyPrime(rng, r)
		&& ec.VerifyPoint(G) && !G.identity && ec.Multiply(r, G).identity
		&& k==(q+2*qSqrt+1)/r && CheckMOVCondition(q, r);
}

template <class EC>
void ECMQVC<EC>::GenerateStaticKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const
{
	Integer x(rng, 1, r-1);
	Point Q = Gpc.Multiply(x);
	x.Encode(privateKey, StaticPrivateKeyLength());
	ec.EncodePoint(publicKey, Q);
}

template <class EC>
void ECMQVC<EC>::GenerateEphemeralKeyPair(RandomNumberGenerator &rng, byte *privateKey, byte *publicKey) const
{
	Integer x(rng, 1, r-1);
	Point Q = Gpc.Multiply(x);
	x.Encode(privateKey, r.ByteCount());
	ec.EncodePoint(privateKey+r.ByteCount(), Q);
	ec.EncodePoint(publicKey, Q);
}

template <class EC>
bool ECMQVC<EC>::Agree(byte *agreedValue, const byte *staticPrivateKey, const byte *ephemeralPrivateKey, const byte *staticOtherPublicKey, const byte *ephemeralOtherPublicKey, bool validateStaticOtherPublicKey) const
{
	Point WW(ec.DecodePoint(staticOtherPublicKey));
	Point VV(ec.DecodePoint(ephemeralOtherPublicKey));
	if (!ec.VerifyPoint(VV) || (validateStaticOtherPublicKey && !ec.VerifyPoint(WW)))
		return false;

	Integer s(staticPrivateKey, StaticPrivateKeyLength());
	Integer u(ephemeralPrivateKey, r.ByteCount());
	Point V(ec.DecodePoint(ephemeralPrivateKey+r.ByteCount()));

	Integer h2 = Integer::Power2((r.BitCount()+1)/2);
	Integer e = ((h2+ConvertToInteger(V.x)%h2)*s+u) % r;
	Point Q = ec.CascadeMultiply(k*e, VV, k*(e*(h2+ConvertToInteger(VV.x)%h2)%r), WW);
	if (Q.identity)
		return false;
	Q.x.Encode(agreedValue, AgreedValueLength());
	return true;
}

template class ECPublicKey<EC2N>;
template class ECPublicKey<ECP>;
template class ECPrivateKey<EC2N>;
template class ECPrivateKey<ECP>;
template class ECDHC<EC2N>;
template class ECDHC<ECP>;
template class ECMQVC<EC2N>;
template class ECMQVC<ECP>;

NAMESPACE_END
