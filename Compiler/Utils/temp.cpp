// Driver fills its vtable
static driver_iface_t g_arm_iface = {
    .api_version = DRIVER_API_VERSION,
    .driver_name = "ARM Driver 1.0",
    .get_target = arm_get_target,
    .set_option = arm_set_option,
    .emit = arm_emit,
    .malloc_fn = NULL, .free_fn = NULL, .alloc_user_ctx = NULL
};

static void diag_sink(diag_severity_t s, const diag_location_t* loc,
    const char* msg, void* ctx) { /* print/log */
}

// Assemble include paths using your include_map API
include_search_path_t inc = { 0 }; /* init and populate... */

frontend_config_t cfg = {
    .mode = FE_MODE_OBJ,
    .lang = {.c_std = "c17", .gnu_ext = false, .freestanding = true },
    .pp = {.defines = NULL, .num_defines = 0, .undefines = NULL, .num_undefines = 0, .inc = &inc },
    .codegen = {.opt_level = 2, .debug_info = true },
    .diag_sink = diag_sink, .diag_user_ctx = NULL,
    .working_dir = "/proj"
};

frontend_ctx_t* fe = frontend_create(/*driver_handle*/arm_self, &g_arm_iface, &cfg);
if (fe && frontend_compile_file(fe, "src/main.c")) {
    /* artifacts delivered via driver_iface.emit() */
}
else {
    /* error path */
}
frontend_destroy(fe);
