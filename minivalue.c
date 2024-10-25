#include "minijson.h"
#include "miniutils.h"

void init_jvalue(JsonValue *v) {
    v->type = JNULL;
}
void free_jvalue(JsonValue *v) {
    switch (v->type) {
    case JARRAY:
        free_jarry(&v->jsonArray);
        return;
    case JSTR:
        free_jstr(&v->jsonStr);
        return;
    case JMAP:
        free_jmap(&v->jsonMap);
        return;
    default:
        return;
    }
}

void jvalue_output(JsonStr *dist, const JsonValue *v, int indent) {
    switch (v->type) {
    case JSTR:
        jstr_sprintf_back(dist, "%s\"%s\"", nspace(2 * indent), jstr_cstr(&v->jsonStr));
        break;
    case JNULL:
        jstr_sprintf_back(dist, "%snull", nspace(2 * indent));
        break;
    case JNUM:
        if (v->jsonNum.isInt) {
            jstr_sprintf_back(dist, "%s%ld", nspace(2 * indent), v->jsonNum.Int64);
        } else {
            jstr_sprintf_back(dist, "%s%lf", nspace(2 * indent), v->jsonNum.Double);
        }

        break;
    case JBOOL:
        if (v->jsonBool.data) {
            jstr_sprintf_back(dist, "%strue", nspace(2 * indent));
        } else {
            jstr_sprintf_back(dist, "%sfalse", nspace(2 * indent));
        }
        break;
    case JARRAY:
        jarray_output(dist, &v->jsonArray, indent);
        break;
    case JMAP:
        jmap_output(dist, &v->jsonMap, indent);
        break;
    }
}

bool Jvalue_isbasic(const JsonValue *v) {
    switch (v->type) {
    case JNULL:
    case JBOOL:
    case JSTR:
    case JNUM:
        return true;
    default:
        return false;
    }
}
