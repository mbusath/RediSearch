#include "extension.h"
#include "redisearch.h"
#include "rmalloc.h"
#include "redismodule.h"
#include "query_result.h"
#include "dep/triemap/triemap.h"

TrieMap *__queryExpanders = NULL;
TrieMap *__scorers = NULL;

void Extensions_Init() {
  __queryExpanders = NewTrieMap();
  __scorers = NewTrieMap();
}

int Ext_RegisterScoringFunction(const char *alias, RSScoringFunction func, void *privdata) {
  if (func == NULL) {
    return REDISEARCH_ERR;
  }
  ExtensionsScoringFunctionCtx *ctx = rm_new(ExtensionsScoringFunctionCtx);
  ctx->privdata = privdata;
  ctx->sf = func;
  if (TrieMap_Find(__scorers, (char *)alias, strlen(alias)) != TRIEMAP_NOTFOUND) {
    return REDISEARCH_ERR;
  }
  TrieMap_Add(__scorers, (char *)alias, strlen(alias), ctx, NULL);
  return REDISEARCH_OK;
}

int Ext_RegisterQueryExpander(const char *alias, RSQueryTokenExpander exp, void *privdata) {
  if (exp == NULL) {
    return REDISEARCH_ERR;
  }
  ExtensionsQueryExpanderCtx *ctx = rm_new(ExtensionsQueryExpanderCtx);
  ctx->privdata = privdata;
  ctx->exp = exp;
  if (TrieMap_Find(__scorers, (char *)alias, strlen(alias)) != TRIEMAP_NOTFOUND) {
    return REDISEARCH_ERR;
  }
  TrieMap_Add(__scorers, (char *)alias, strlen(alias), ctx, NULL);
  return REDISEARCH_OK;
}

int Extension_Load(const char *name, RSExtensionInitFunc func) {
  printf("Loading extension %s\n", name);
  RSExtensionCtx ctx = {
      .RegisterScoringFunction = Ext_RegisterScoringFunction,
      .RegisterQueryExpander = Ext_RegisterQueryExpander,
  };

  return func(&ctx);
}

ExtensionsScoringFunctionCtx *Extensions_GetScoringFunction(const char *name) {

  void *p = TrieMap_Find(__scorers, (char*)name, strlen(name));
  if (p && p != TRIEMAP_NOTFOUND) {
    return (ExtensionsScoringFunctionCtx *)p;
  }
  return NULL;
}

ExtensionsQueryExpanderCtx *Extensions_GetQueryExpander(const char *name) {
  void *p = TrieMap_Find(__queryExpanders, (char*)name, strlen(name));
  if (p && p != TRIEMAP_NOTFOUND) {
    return (ExtensionsQueryExpanderCtx *)p;
  }
  return NULL;
}


int Ext_ScorerGetSlop(RSQueryResult *r) {
  return Query
}