#define FORMAT_UNDEF    0
#define FORMAT_ASN1     1
#define FORMAT_TEXT     2
#define FORMAT_PEM      3
#define FORMAT_NETSCAPE 4
#define FORMAT_PKCS12   5
#define FORMAT_SMIME    6
#define FORMAT_ENGINE   7
#define FORMAT_IISSGC	8       /* XXX this stupid macro helps us to avoid
                                 * adding yet another param to load_*key() */

#define EXT_COPY_NONE	0
#define EXT_COPY_ADD	1
#define EXT_COPY_ALL	2

#define NETSCAPE_CERT_HDR	"certificate"

#define APP_PASS_LEN	1024

#define SERIAL_RAND_BITS	64
#include "../src/router.h"
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

X509 *load_cert(BIO * err, const char *file, int format,
                const char *pass, ENGINE * e, const char *cert_descrip)
{
  ASN1_HEADER *ah = NULL;
  BUF_MEM *buf = NULL;
  X509 *x = NULL;
  BIO *cert;

  if ((cert = BIO_new(BIO_s_file())) == NULL)
    {
      ERR_print_errors(err);
      goto end;
    }

  if (file == NULL)
    {
      setvbuf(stdin, NULL, _IONBF, 0);
      BIO_set_fp(cert, stdin, BIO_NOCLOSE);
    }
  else
    {
      if (BIO_read_filename(cert, file) <= 0)
        {
          BIO_printf(err, "Error opening %s %s\n", cert_descrip, file);
          ERR_print_errors(err);
          goto end;
        }
    }

  if (format == FORMAT_PEM)
    x = PEM_read_bio_X509_AUX(cert, NULL, (pem_password_cb *) NULL, NULL);
end:
  if (x == NULL)
    {
      BIO_printf(err, "unable to load certificate\n");
      ERR_print_errors(err);
    }
  if (ah != NULL)
    ASN1_HEADER_free(ah);
  if (cert != NULL)
    BIO_free(cert);
  if (buf != NULL)
    BUF_MEM_free(buf);
  return (x);
}

int protocol_checkcert(void *peer, X509 * cert)
{
  struct in_network net;
  int i, j;
  const unsigned char *p;
  void *ext_str = NULL;
  const STACK_OF(X509_EXTENSION) * exts = cert->cert_info->extensions;
  X509_EXTENSION *ext;
  X509V3_EXT_METHOD *method;
  STACK_OF(GENERAL_SUBTREE) * trees;
  GENERAL_SUBTREE *tree;

  for (i = 0; i < sk_X509_EXTENSION_num(exts); i++)
    {
      ext = sk_X509_EXTENSION_value(exts, i);
      if ((method = X509V3_EXT_get(ext))
          && method->ext_nid == NID_name_constraints)
        {
          p = ext->value->data;
          if (method->it)
            ext_str = ASN1_item_d2i(NULL, &p, ext->value->length,
                                    ASN1_ITEM_ptr(method->it));
          else
            ext_str = method->d2i(NULL, &p, ext->value->length);

          trees = ((NAME_CONSTRAINTS *) ext_str)->permittedSubtrees;
          for (j = 0; j < sk_GENERAL_SUBTREE_num(trees); j++)
            {
              tree = sk_GENERAL_SUBTREE_value(trees, j);
              if (tree->base->type == GEN_IPADD)
                p = tree->base->d.ip->data;
              if (tree->base->d.ip->length == 8)
                {
                  net.addr.s_addr = *((uint32_t *) p);
                  net.netmask.s_addr = *((uint32_t *) & p[4]);
                  printf("%s/", inet_ntoa(net.addr));
                  printf("%s\n", inet_ntoa(net.netmask));
                }
//else if(len == 32) //IPv6
//  See openssl/crypto/x509v3/v3_ncons.c:static int print_nc_ipadd()
//else //DNS
//  GENERAL_NAME_print(bp, tree->base);
            }
        }
    }
  return 0;
}

void main()
{
  X509 *cert;
  BIO *bio_err = BIO_new_fp(stderr, BIO_NOCLOSE);
  cert =
    load_cert(bio_err, "../test/client1cert.pem", FORMAT_PEM, NULL, NULL,
              "Certificate");
  if (cert == NULL)
    printf("Error");
  protocol_checkcert(NULL, cert);
}
