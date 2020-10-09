static int pkg_list(const char *name) {
    str p = {0};

    str_cat(&p, DB_DIR);
    str_cat(&p, "/");
    str_cat(&p, name);

    int ret = is_dir(p.buf);

    str_free(&p);

    return !ret;
}

static char *repo_find(const char *pkg) {
    if (!repos) {
        repo_init();
    }

    for (size_t i = 0; i < vec_size(repos); ++i) {
        str query = {0};

        str_cat(&query, repos[i]);
        str_cat(&query, "/");
        str_cat(&query, pkg);

        int ret = is_dir(query.buf);

        if (ret == 0) {
            return query.buf;
        }

        str_free(&query);
    }

    return NULL;
}

