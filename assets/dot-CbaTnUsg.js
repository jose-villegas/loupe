var e=e=>{switch(e){case`index`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=index,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    developer [height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">Developer</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">Person</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Opens a scene, steps through its frames,<BR/>moves its objects and saves them back.</FONT></TD></TR></TABLE>>,
        likec4_id=developer,
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    loupe [height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">Loupe</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe is a frame debugger and scene editor<BR/>for the autana engine.</FONT></TD></TR></TABLE>>,
        likec4_id=loupe,
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    developer -> loupe [arrowhead=normal,
        likec4_id="162hym7",
        minlen=1,
        style=dashed,
        xlabel=<<TABLE BORDER="0" CELLPADDING="3" CELLSPACING="0" BGCOLOR="#18191BA0"><TR><TD ALIGN="TEXT" BALIGN="LEFT"><FONT POINT-SIZE="14">opens scenes, edits, renders</FONT></TD></TR></TABLE>>];
    ci [color="#525252",
        fillcolor="#737373",
        fontcolor="#fafafa",
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">GitHub Actions</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#d4d4d4">Software system</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#d4d4d4">Builds and tests Loupe on every pull request<BR/>and publishes its releases.</FONT></TD></TR></TABLE>>,
        likec4_id=ci,
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    ci -> loupe [arrowhead=normal,
        likec4_id="1cixuar",
        minlen=1,
        style=dashed,
        xlabel=<<TABLE BORDER="0" CELLPADDING="3" CELLSPACING="0" BGCOLOR="#18191BA0"><TR><TD ALIGN="TEXT" BALIGN="LEFT"><FONT POINT-SIZE="14">builds, tests, releases</FONT></TD></TR></TABLE>>];
    autana [color="#525252",
        fillcolor="#737373",
        fontcolor="#fafafa",
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">autana</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#d4d4d4">Software system</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#d4d4d4">The engine whose drawing code Loupe runs,<BR/>scenes and packs included.</FONT></TD></TR></TABLE>>,
        likec4_id=autana,
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    loupe -> autana [arrowhead=normal,
        likec4_id="12q1fk7",
        minlen=1,
        style=dashed,
        xlabel=<<TABLE BORDER="0" CELLPADDING="3" CELLSPACING="0" BGCOLOR="#18191BA0"><TR><TD ALIGN="TEXT" BALIGN="LEFT"><FONT POINT-SIZE="14">draws with its code, builds its packs</FONT></TD></TR></TABLE>>];
}
`;case`containers`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=containers,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_loupe {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>LOUPE</B></FONT>>,
            likec4_depth=1,
            likec4_id=loupe,
            likec4_level=0,
            margin=32,
            style=filled
        ];
        loupe [height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">Loupe</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">C++20 · SDL3 · Vulkan · Dear ImGui</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Opens a scene in the window, or the window's<BR/>start panel without one; with -o or --video,<BR/>writes the scene's frames and exits; with<BR/>--frame or...</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    developer [height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">Developer</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">Person</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Opens a scene, steps through its frames,<BR/>moves its objects and saves them back.</FONT></TD></TR></TABLE>>,
        likec4_id=developer,
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    developer -> loupe [arrowhead=normal,
        likec4_id=holtg2,
        minlen=1,
        style=dashed,
        xlabel=<<TABLE BORDER="0" CELLPADDING="3" CELLSPACING="0" BGCOLOR="#18191BA0"><TR><TD ALIGN="TEXT" BALIGN="LEFT"><FONT POINT-SIZE="14">opens scenes, edits, renders</FONT></TD></TR></TABLE>>];
    ci [color="#525252",
        fillcolor="#737373",
        fontcolor="#fafafa",
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">GitHub Actions</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#d4d4d4">Software system</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#d4d4d4">Builds and tests Loupe on every pull request<BR/>and publishes its releases.</FONT></TD></TR></TABLE>>,
        likec4_id=ci,
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    ci -> loupe [arrowhead=normal,
        likec4_id="1h7i13i",
        minlen=1,
        style=dashed,
        xlabel=<<TABLE BORDER="0" CELLPADDING="3" CELLSPACING="0" BGCOLOR="#18191BA0"><TR><TD ALIGN="TEXT" BALIGN="LEFT"><FONT POINT-SIZE="14">builds, tests, releases</FONT></TD></TR></TABLE>>];
    autana [color="#525252",
        fillcolor="#737373",
        fontcolor="#fafafa",
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">autana</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#d4d4d4">Software system</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#d4d4d4">The engine whose drawing code Loupe runs,<BR/>scenes and packs included.</FONT></TD></TR></TABLE>>,
        likec4_id=autana,
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    loupe -> autana [arrowhead=normal,
        likec4_id="1qf1kei",
        minlen=1,
        style=dashed,
        xlabel=<<TABLE BORDER="0" CELLPADDING="3" CELLSPACING="0" BGCOLOR="#18191BA0"><TR><TD ALIGN="TEXT" BALIGN="LEFT"><FONT POINT-SIZE="14">draws with its code, builds its packs</FONT></TD></TR></TABLE>>];
}
`;case`loupe`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=loupe,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_loupe {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>LOUPE</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe",
            likec4_level=0,
            margin=40,
            style=filled
        ];
        c_path [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">path</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 404 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">A scene's camera path from the c</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_path",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_storyboard [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">storyboard</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 751 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_storyboard",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_window [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">20 files · 3514 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_editor [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 1052 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_editor",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_engine [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 994 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_log [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">log</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 611 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe's log: one sink every line</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_log",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_update [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 847 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_update",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_src [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_path -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=nher8l,
        style=solid];
    c_storyboard -> c_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=tftjb3,
        minlen=1,
        style=solid];
    c_window -> c_editor [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=mw6u1z,
        style=solid];
    c_window -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=n30u14,
        style=solid];
    c_window -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=deggqq,
        minlen=1,
        style=solid];
    c_window -> c_update [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=wuxb7b,
        style=solid];
    c_editor -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="3vhivd",
        style=solid];
    c_engine -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=zr62cm,
        style=solid];
    c_update -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1ajv1uh",
        style=solid];
    c_src -> c_path [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1kireed",
        style=solid];
    c_src -> c_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="18fjin8",
        style=solid];
}
`;case`loupe_all`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=loupe_all,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_loupe {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>LOUPE</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe",
            likec4_level=0,
            margin=40,
            style=filled
        ];
        c_tests [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">tests</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">0 files · 0 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_tests",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_path [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">path</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 404 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">A scene's camera path from the c</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_path",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_storyboard [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">storyboard</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 751 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_storyboard",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_window [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">20 files · 3514 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_editor [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 1052 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_editor",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_update [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 847 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_update",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_engine [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 994 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_src [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_log [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">log</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 611 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe's log: one sink every line</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_log",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_tests -> c_path [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1y7tcgy",
        style=solid];
    c_tests -> c_storyboard [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=yucqnq,
        style=solid];
    c_tests -> c_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=v6ve8z,
        style=solid];
    c_tests -> c_editor [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=jv466m,
        style=solid];
    c_tests -> c_update [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=ttweqm,
        style=solid];
    c_tests -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=jqu1k1,
        style=solid];
    c_tests -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1bbumb1",
        style=solid];
    c_tests -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1bbu3d7",
        style=solid];
    c_path -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1xktug9",
        style=solid];
    c_path -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=nher8l,
        style=solid];
    c_path -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=nhelc3,
        style=solid];
    c_storyboard -> c_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=tftjb3,
        style=solid];
    c_storyboard -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1ey23n5",
        style=solid];
    c_window -> c_editor [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=mw6u1z,
        style=solid];
    c_window -> c_update [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=wuxb7b,
        style=solid];
    c_window -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=n30u14,
        style=solid];
    c_window -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=degrfo,
        style=solid];
    c_window -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=deggqq,
        style=solid];
    c_editor -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=d4l2md,
        style=solid];
    c_editor -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="3vhivd",
        style=solid];
    c_update -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1ajv1uh",
        style=solid];
    c_update -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1ajvfpb",
        style=solid];
    c_engine -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=zr62cm,
        style=solid];
    c_engine -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=zr5ohs,
        style=solid];
    c_src -> c_path [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1kireed",
        style=solid];
    c_src -> c_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="18fjin8",
        style=solid];
    c_src -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=u51y92,
        style=solid];
    c_src -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1d2dze4",
        style=solid];
}
`;case`c_editor`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_editor,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_editor {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>EDITOR</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_editor",
            likec4_level=0,
            margin=40,
            style=filled
        ];
        m_scene_tool [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_editor",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">scene_tool</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">scene_tool.cpp, scene_tool.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The editor's calls into autana's</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_editor.m_scene_tool",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_doc [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_editor",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">doc</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">doc.cpp, doc.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">What the editor holds and saves:</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_editor.m_doc",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_camera_path [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_editor",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">camera_path</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">camera_path.cpp, camera_path.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">A camera path as its .keys.toml</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_editor.m_camera_path",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_placement [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_editor",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">placement</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">placement.cpp, placement.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">A Placement both ways: to the en</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_editor.m_placement",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_window [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">20 files · 3514 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_window",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_window -> m_scene_tool [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="300m9u",
        minlen=1,
        style=solid];
    m_scene_tool -> m_doc [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=quj5y3,
        style=solid,
        weight=2];
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_src",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_scene_tool -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=w5k7qk,
        minlen=1,
        style=solid];
    m_doc -> m_camera_path [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="18s10zt",
        style=solid];
    m_camera_path -> m_placement [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1xjgphk",
        style=solid,
        weight=2];
    m_placement -> m_doc [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1img2z",
        style=solid,
        weight=2];
    c_engine [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 994 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_engine",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_placement -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="68o1f4",
        minlen=1,
        style=solid];
}
`;case`c_engine`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_engine,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_engine {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>ENGINE</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_engine",
            likec4_level=0,
            margin=40,
            style=filled
        ];
        m_frames [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_engine",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">frames</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">frames.cpp, frames.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Gfx's frame from a provider, dra</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine.m_frames",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_path_curve [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_engine",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">path_curve</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">path_curve.cpp, path_curve.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">A camera path as the clip autana</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine.m_path_curve",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_engine_frame [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_engine",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine_frame</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">engine_frame.cpp, engine_frame.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The z mirror between autana's fr</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine.m_engine_frame",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_autana [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_engine",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">autana</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">autana.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The one place Loupe includes aut</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine.m_autana",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_pose [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_engine",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">pose</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">pose.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Autana's packages (launcher/pack</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine.m_pose",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_editor [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 1052 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_editor",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_editor -> m_frames [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="197q00n",
        style=solid];
    c_editor -> m_path_curve [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1sahqbg",
        style=solid];
    c_path [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">path</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 404 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">A scene's camera path from the c</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_path",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_path -> m_frames [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1djv62j",
        minlen=1,
        style=solid];
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_src",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_src -> m_frames [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1pzjbo",
        style=solid];
    c_src -> m_autana [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="2bjgdw",
        style=solid];
    c_window [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">20 files · 3514 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_window",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_window -> m_frames [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1pi6fcq",
        style=solid];
    c_window -> m_engine_frame [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=xoyw60,
        style=solid];
    m_frames -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1yauvf8",
        style=solid];
    c_log [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">log</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 611 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe's log: one sink every line</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_log",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_frames -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1yavhiq",
        minlen=1,
        style=solid];
    m_frames -> m_pose [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=l8yigd,
        style=solid,
        weight=2];
    m_path_curve -> m_autana [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="3b9z5",
        style=solid,
        weight=2];
    m_path_curve -> m_pose [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1qw2i6e",
        style=solid,
        weight=2];
    m_engine_frame -> m_pose [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=vczyof,
        style=solid,
        weight=2];
}
`;case`c_log`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_log,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_log {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>LOG</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_log",
            likec4_level=0,
            margin=32,
            style=filled
        ];
        m_log [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">log</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">log.cpp, log.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Loupe's log: one sink every line</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_log.m_log",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_engine [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 994 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_engine",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_engine -> m_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1lu2zi0",
        minlen=1,
        style=solid];
    c_path [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">path</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 404 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">A scene's camera path from the c</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_path",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_path -> m_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=hyjvi3,
        minlen=1,
        style=solid];
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_src",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_src -> m_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="9678no",
        minlen=1,
        style=solid];
    c_update [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 847 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_update",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_update -> m_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=sjdrlz,
        minlen=1,
        style=solid];
    c_window [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">20 files · 3514 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_window",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_window -> m_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="2sa30a",
        minlen=1,
        style=solid];
}
`;case`c_path`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_path,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_path {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>PATH</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_path",
            likec4_level=0,
            margin=32,
            style=filled
        ];
        m_path [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">path</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">path.cpp, path.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">A scene's camera path from the c</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_path.m_path",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_src",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_src -> m_path [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=y4y184,
        style=solid];
    m_path -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=sucut0,
        style=solid];
    c_engine [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 994 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_engine",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_path -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1wel3x4",
        minlen=1,
        style=solid];
    c_log [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">log</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 611 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe's log: one sink every line</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_log",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_path -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=suda6q,
        minlen=1,
        style=solid];
}
`;case`c_src`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_src,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_src {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>SRC</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_src",
            likec4_level=0,
            margin=40,
            style=filled
        ];
        m_main [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">main</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">main.cpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Opens a scene in the window, or</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_main",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_options [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_src",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">options</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">options.cpp, options.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">What one Loupe run is asked for:</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_options",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_scratch [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">scratch</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">scratch.cpp, scratch.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">A folder of a run's own files, u</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_scratch",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_version [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">version</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">version.cpp, version.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Which Loupe this is: its version</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_version",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_library [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_src",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">library</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">library.cpp, library.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The scenes the window's start pa</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_library",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_pack [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_src",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">pack</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">pack.cpp, pack.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">A scene's asset pack: a .apak as</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_pack",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_tools [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_src",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">tools</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">tools.cpp, tools.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The programs a build with the pa</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_tools",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_process [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_src",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">process</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">process.cpp, process.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Other processes: the tools loupe</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_process",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_paths [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_src",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">paths</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">paths.cpp, paths.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">A path as UTF-8 text and back: t</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src.m_paths",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_editor [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 1052 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_editor",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_editor -> m_tools [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1fs7eym",
        minlen=1,
        style=solid];
    c_engine [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 994 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_engine",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_engine -> m_options [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=vpu8y2,
        style=solid];
    c_storyboard [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">storyboard</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 751 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_storyboard",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_storyboard -> m_options [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=vj5r19,
        minlen=1,
        style=solid];
    c_update [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 847 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_update",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_update -> m_options [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1yoq25x",
        style=solid];
    c_update -> m_scratch [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=x06hmh,
        style=solid];
    c_update -> m_version [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="10shtr",
        style=solid];
    c_path [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">path</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 404 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">A scene's camera path from the c</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_path",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_main -> c_path [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1kfagdu",
        style=solid];
    c_window [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">20 files · 3514 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_window",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_main -> c_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1xulrlf",
        style=solid];
    c_path -> m_scratch [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="3x577p",
        style=solid];
    c_path -> m_pack [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=gkl9q8,
        style=solid];
    c_window -> m_scratch [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=wnwkn8,
        style=solid];
    c_window -> m_version [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1uxgcg2",
        style=solid];
    c_window -> m_library [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1ct4ekn",
        style=solid];
    m_options -> m_pack [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1p19sir",
        style=solid,
        weight=2];
    m_library -> m_pack [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1cmduks",
        style=solid,
        weight=2];
    m_pack -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=wfjrgj,
        style=solid];
    m_pack -> m_tools [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="8eq648",
        style=solid,
        weight=2];
    m_tools -> m_process [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1vk70a2",
        style=solid,
        weight=2];
    m_process -> m_paths [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1huqon3",
        minlen=1,
        style=solid,
        weight=2];
    c_log [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">log</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 611 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe's log: one sink every line</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_log",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_process -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=hdfszf,
        minlen=1,
        style=solid];
}
`;case`c_storyboard`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_storyboard,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_storyboard {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>STORYBOARD</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_storyboard",
            likec4_level=0,
            margin=40,
            style=filled
        ];
        m_main [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_storyboard",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">main</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">main.cpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Runs a recipe's loupe.* steps on</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_storyboard.m_main",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_runner [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_storyboard",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">runner</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">runner.cpp, runner.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">ImVec2's arithmetic, for the cro</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_storyboard.m_runner",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_recipe [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_storyboard",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">recipe</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">recipe.cpp, recipe.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">A recipe: the YAML list of steps</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_storyboard.m_recipe",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_config [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">storyboard/config</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">1 file · 6 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Dear ImGui's configuration in a</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_storyboard.c_config",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    m_main -> m_runner [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1v1jzw1",
        minlen=1,
        style=solid,
        weight=2];
    m_runner -> m_recipe [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1fo89g2",
        style=solid,
        weight=2];
    c_window [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">20 files · 3514 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_window",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_runner -> c_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=kdt8b7,
        minlen=1,
        style=solid];
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_src",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_recipe -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1crprt1",
        minlen=1,
        style=solid];
}
`;case`c_storyboard_config`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_storyboard_config,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_config {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>STORYBOARD/CONFIG</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_storyboard.c_config",
            likec4_level=0,
            margin=32,
            style=filled
        ];
        m_imgui_config [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">imgui_config</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">imgui_config.h</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Dear ImGui's configuration in a</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_storyboard.c_config.m_imgui_config",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
}
`;case`c_tests`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_tests,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        label="\\N",
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
}
`;case`c_update`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_update,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_update {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>UPDATE</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_update",
            likec4_level=0,
            margin=40,
            style=filled
        ];
        m_update [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">update.cpp, update.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_update.m_update",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_sha256 [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">sha256</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">sha256.cpp, sha256.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">For checking a download against</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_update.m_sha256",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_window [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">20 files · 3514 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_window",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_window -> m_update [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=k6lboa,
        minlen=1,
        style=solid];
    m_update -> m_sha256 [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=e1wvyo,
        minlen=0,
        style=solid,
        weight=2];
    c_log [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">log</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 611 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe's log: one sink every line</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_log",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_update -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1e17mqa",
        minlen=1,
        style=solid];
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_src",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_update -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1e177ck",
        minlen=1,
        style=solid];
}
`;case`c_window`:return`digraph {
    graph [TBbalance=min,
        bgcolor=transparent,
        compound=true,
        fontname=Arial,
        fontsize=20,
        labeljust=l,
        labelloc=t,
        layout=dot,
        likec4_viewId=c_window,
        nodesep=1.528,
        outputorder=nodesfirst,
        pad=0.209,
        rankdir=TB,
        ranksep=1.667,
        splines=ortho
    ];
    node [color="#2563eb",
        fillcolor="#3b82f6",
        fontcolor="#eff6ff",
        fontname=Arial,
        penwidth=0,
        shape=rect,
        style=filled
    ];
    edge [arrowsize=0.75,
        color="#8D8D8D",
        fontcolor="#C9C9C9",
        fontname=Arial,
        fontsize=14,
        penwidth=2,
        style=""
    ];
    subgraph cluster_c_window {
        graph [color="#1b3d88",
            fillcolor="#194b9e",
            label=<<FONT POINT-SIZE="11" COLOR="#bfdbfeb3"><B>WINDOW</B></FONT>>,
            likec4_depth=1,
            likec4_id="loupe.loupe.c_window",
            likec4_level=0,
            margin=40,
            style=filled
        ];
        m_run_window [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">run_window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">run_window.cpp, run_window.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_run_window",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_console [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">console</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">console.cpp, console.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The Console panel: the log's lin</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_console",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_editor [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">editor.cpp, editor.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The scene editor's panels over a</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_editor",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_files [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">files</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">files.cpp, files.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The window's files through SDL:</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_files",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_frame_texture [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">frame_texture</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">frame_texture.cpp, frame_texture.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The engine's frame as one textur</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_frame_texture",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_observer [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">observer</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">observer.cpp, observer.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The observer: the person's own c</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_observer",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_path_editor [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">path_editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">path_editor.cpp, path_editor.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The editor's camera paths: the s</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_path_editor",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_gpu [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">gpu</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">gpu.cpp, gpu.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">After imgui's examples/example_s</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_gpu",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_panels [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">panels</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">panels.cpp, panels.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The window's panels.</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_panels",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_fonts [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">fonts</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">fonts.cpp, fonts.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The panels' typefaces by role: T</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_fonts",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1274 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_src",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_src -> m_run_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=zyy7s2,
        style=solid];
    c_storyboard [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">storyboard</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 751 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_storyboard",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_storyboard -> m_run_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="8wujkp",
        minlen=1,
        style=solid];
    m_run_window -> m_console [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1c8vlqf",
        style=solid,
        weight=2];
    m_run_window -> m_editor [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="26xyhd",
        style=solid,
        weight=2];
    m_run_window -> m_files [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=d4d2h1,
        style=solid,
        weight=2];
    m_run_window -> m_frame_texture [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1bfg7h",
        style=solid,
        weight=2];
    m_run_window -> m_observer [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="14ud1co",
        style=solid,
        weight=2];
    c_update [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 847 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_update",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_run_window -> c_update [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1qeyl4h",
        minlen=1,
        style=solid];
    m_console -> m_fonts [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1wwnv6l",
        style=solid,
        weight=2];
    c_log [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">log</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">2 files · 611 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe's log: one sink every line</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_log",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_console -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1nek8ax",
        style=solid];
    m_editor -> m_path_editor [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=awgjiw,
        style=solid,
        weight=2];
    m_files -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="19rm5m5",
        style=solid];
    m_frame_texture -> m_gpu [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1mtv6lx",
        style=solid,
        weight=2];
    c_engine [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 994 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_engine",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_frame_texture -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1gpmbah",
        style=solid];
    m_observer -> c_engine [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1m1tsbg",
        style=solid];
    m_path_editor -> m_panels [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="727z18",
        style=solid,
        weight=2];
    c_editor [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">8 files · 1052 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_editor",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_path_editor -> c_editor [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="2m5294",
        minlen=1,
        style=solid];
    m_gpu -> m_fonts [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1ttrfvs",
        style=solid,
        weight=2];
    m_gpu -> c_log [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=uuwqqk,
        style=solid];
    m_panels -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1f6f0d9",
        style=solid];
}
`;default:throw Error(`Unknown viewId: `+e)}},t=e=>{switch(e){case`index`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="796pt" height="810pt"
 viewBox="0.00 0.00 796.00 810.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 795.05)">
<!-- developer -->
<g id="node1" class="node">
<title>developer</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-780 0,-780 0,-600 320.04,-600 320.04,-780"/>
<text xml:space="preserve" text-anchor="start" x="114.44" y="-711.8" font-family="Arial" font-size="20.00" fill="#eff6ff">Developer</text>
<text xml:space="preserve" text-anchor="start" x="139.43" y="-690.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">Person</text>
<text xml:space="preserve" text-anchor="start" x="23.28" y="-669.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Opens a scene, steps through its frames,</text>
<text xml:space="preserve" text-anchor="start" x="27.46" y="-651.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">moves its objects and saves them back.</text>
</g>
<!-- loupe -->
<g id="node2" class="node">
<title>loupe</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="546.26,-480 211.78,-480 211.78,-300 546.26,-300 546.26,-480"/>
<text xml:space="preserve" text-anchor="start" x="351.21" y="-402" font-family="Arial" font-size="20.00" fill="#eff6ff">Loupe</text>
<text xml:space="preserve" text-anchor="start" x="231.84" y="-379" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe is a frame debugger and scene editor</text>
<text xml:space="preserve" text-anchor="start" x="306.05" y="-361" font-family="Arial" font-size="15.00" fill="#bfdbfe">for the autana engine.</text>
</g>
<!-- ci -->
<g id="node3" class="node">
<title>ci</title>
<polygon fill="#737373" stroke="#525252" stroke-width="0" points="765.67,-780 430.37,-780 430.37,-600 765.67,-600 765.67,-780"/>
<text xml:space="preserve" text-anchor="start" x="531.33" y="-711.8" font-family="Arial" font-size="20.00" fill="#fafafa">GitHub Actions</text>
<text xml:space="preserve" text-anchor="start" x="549.98" y="-690.8" font-family="Arial" font-size="13.00" fill="#d4d4d4">Software system</text>
<text xml:space="preserve" text-anchor="start" x="450.43" y="-669.2" font-family="Arial" font-size="15.00" fill="#d4d4d4">Builds and tests Loupe on every pull request</text>
<text xml:space="preserve" text-anchor="start" x="509.63" y="-651.2" font-family="Arial" font-size="15.00" fill="#d4d4d4">and publishes its releases.</text>
</g>
<!-- autana -->
<g id="node4" class="node">
<title>autana</title>
<polygon fill="#737373" stroke="#525252" stroke-width="0" points="548.76,-180 209.28,-180 209.28,0 548.76,0 548.76,-180"/>
<text xml:space="preserve" text-anchor="start" x="348.43" y="-111.8" font-family="Arial" font-size="20.00" fill="#fafafa">autana</text>
<text xml:space="preserve" text-anchor="start" x="330.98" y="-90.8" font-family="Arial" font-size="13.00" fill="#d4d4d4">Software system</text>
<text xml:space="preserve" text-anchor="start" x="229.33" y="-69.2" font-family="Arial" font-size="15.00" fill="#d4d4d4">The engine whose drawing code Loupe runs,</text>
<text xml:space="preserve" text-anchor="start" x="286.88" y="-51.2" font-family="Arial" font-size="15.00" fill="#d4d4d4">scenes and packs included.</text>
</g>
<!-- developer&#45;&gt;loupe -->
<g id="edge1" class="edge">
<title>developer&#45;&gt;loupe</title>
<path fill="none" stroke="#8d8d8d" stroke-width="2" stroke-dasharray="5,2" d="M265.91,-600.4C265.91,-600.4 265.91,-490.45 265.91,-490.45"/>
<polygon fill="#8d8d8d" stroke="#8d8d8d" stroke-width="2" points="268.54,-490.45 265.91,-482.95 263.29,-490.45 268.54,-490.45"/>
<polygon fill="#18191b" fill-opacity="0.627451" stroke="none" points="80.92,-545.42 80.92,-568.22 265.91,-568.22 265.91,-545.42 80.92,-545.42"/>
<text xml:space="preserve" text-anchor="start" x="83.92" y="-551.22" font-family="Arial" font-size="14.00" fill="#c9c9c9">opens scenes, edits, renders</text>
</g>
<!-- loupe&#45;&gt;autana -->
<g id="edge3" class="edge">
<title>loupe&#45;&gt;autana</title>
<path fill="none" stroke="#8d8d8d" stroke-width="2" stroke-dasharray="5,2" d="M379.02,-300.4C379.02,-300.4 379.02,-190.45 379.02,-190.45"/>
<polygon fill="#8d8d8d" stroke="#8d8d8d" stroke-width="2" points="381.65,-190.45 379.02,-182.95 376.4,-190.45 381.65,-190.45"/>
<polygon fill="#18191b" fill-opacity="0.627451" stroke="none" points="152.04,-245.42 152.04,-268.22 379.02,-268.22 379.02,-245.42 152.04,-245.42"/>
<text xml:space="preserve" text-anchor="start" x="155.04" y="-251.22" font-family="Arial" font-size="14.00" fill="#c9c9c9">draws with its code, builds its packs</text>
</g>
<!-- ci&#45;&gt;loupe -->
<g id="edge2" class="edge">
<title>ci&#45;&gt;loupe</title>
<path fill="none" stroke="#8d8d8d" stroke-width="2" stroke-dasharray="5,2" d="M488.31,-600.4C488.31,-600.4 488.31,-490.45 488.31,-490.45"/>
<polygon fill="#8d8d8d" stroke="#8d8d8d" stroke-width="2" points="490.94,-490.45 488.31,-482.95 485.69,-490.45 490.94,-490.45"/>
<polygon fill="#18191b" fill-opacity="0.627451" stroke="none" points="347.69,-545.42 347.69,-568.22 488.31,-568.22 488.31,-545.42 347.69,-545.42"/>
<text xml:space="preserve" text-anchor="start" x="350.69" y="-551.22" font-family="Arial" font-size="14.00" fill="#c9c9c9">builds, tests, releases</text>
</g>
</g>
</svg>
`;case`containers`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="796pt" height="810pt"
 viewBox="0.00 0.00 796.00 810.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 795.05)">
<g id="clust1" class="cluster">
<title>cluster_loupe</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="173.02,-268 173.02,-533.2 585.02,-533.2 585.02,-268 173.02,-268"/>
<text xml:space="preserve" text-anchor="start" x="181.02" y="-520.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">LOUPE</text>
</g>
<!-- loupe -->
<g id="node1" class="node">
<title>loupe</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="552.68,-480 205.36,-480 205.36,-300 552.68,-300 552.68,-480"/>
<text xml:space="preserve" text-anchor="start" x="351.21" y="-429.8" font-family="Arial" font-size="20.00" fill="#eff6ff">Loupe</text>
<text xml:space="preserve" text-anchor="start" x="268.47" y="-408.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">C++20 · SDL3 · Vulkan · Dear ImGui</text>
<text xml:space="preserve" text-anchor="start" x="225.42" y="-387.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Opens a scene in the window, or the window&#39;s</text>
<text xml:space="preserve" text-anchor="start" x="241.45" y="-369.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">start panel without one; with &#45;o or &#45;&#45;video,</text>
<text xml:space="preserve" text-anchor="start" x="245.03" y="-351.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">writes the scene&#39;s frames and exits; with</text>
<text xml:space="preserve" text-anchor="start" x="339.85" y="-333.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">&#45;&#45;frame or...</text>
</g>
<!-- developer -->
<g id="node2" class="node">
<title>developer</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-780 0,-780 0,-600 320.04,-600 320.04,-780"/>
<text xml:space="preserve" text-anchor="start" x="114.44" y="-711.8" font-family="Arial" font-size="20.00" fill="#eff6ff">Developer</text>
<text xml:space="preserve" text-anchor="start" x="139.43" y="-690.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">Person</text>
<text xml:space="preserve" text-anchor="start" x="23.28" y="-669.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Opens a scene, steps through its frames,</text>
<text xml:space="preserve" text-anchor="start" x="27.46" y="-651.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">moves its objects and saves them back.</text>
</g>
<!-- ci -->
<g id="node3" class="node">
<title>ci</title>
<polygon fill="#737373" stroke="#525252" stroke-width="0" points="765.67,-780 430.37,-780 430.37,-600 765.67,-600 765.67,-780"/>
<text xml:space="preserve" text-anchor="start" x="531.33" y="-711.8" font-family="Arial" font-size="20.00" fill="#fafafa">GitHub Actions</text>
<text xml:space="preserve" text-anchor="start" x="549.98" y="-690.8" font-family="Arial" font-size="13.00" fill="#d4d4d4">Software system</text>
<text xml:space="preserve" text-anchor="start" x="450.43" y="-669.2" font-family="Arial" font-size="15.00" fill="#d4d4d4">Builds and tests Loupe on every pull request</text>
<text xml:space="preserve" text-anchor="start" x="509.63" y="-651.2" font-family="Arial" font-size="15.00" fill="#d4d4d4">and publishes its releases.</text>
</g>
<!-- autana -->
<g id="node4" class="node">
<title>autana</title>
<polygon fill="#737373" stroke="#525252" stroke-width="0" points="548.76,-180 209.28,-180 209.28,0 548.76,0 548.76,-180"/>
<text xml:space="preserve" text-anchor="start" x="348.43" y="-111.8" font-family="Arial" font-size="20.00" fill="#fafafa">autana</text>
<text xml:space="preserve" text-anchor="start" x="330.98" y="-90.8" font-family="Arial" font-size="13.00" fill="#d4d4d4">Software system</text>
<text xml:space="preserve" text-anchor="start" x="229.33" y="-69.2" font-family="Arial" font-size="15.00" fill="#d4d4d4">The engine whose drawing code Loupe runs,</text>
<text xml:space="preserve" text-anchor="start" x="286.88" y="-51.2" font-family="Arial" font-size="15.00" fill="#d4d4d4">scenes and packs included.</text>
</g>
<!-- loupe&#45;&gt;autana -->
<g id="edge3" class="edge">
<title>loupe&#45;&gt;autana</title>
<path fill="none" stroke="#8d8d8d" stroke-width="2" stroke-dasharray="5,2" d="M379.02,-300.4C379.02,-300.4 379.02,-190.45 379.02,-190.45"/>
<polygon fill="#8d8d8d" stroke="#8d8d8d" stroke-width="2" points="381.65,-190.45 379.02,-182.95 376.4,-190.45 381.65,-190.45"/>
<polygon fill="#18191b" fill-opacity="0.627451" stroke="none" points="152.04,-245.42 152.04,-268.22 379.02,-268.22 379.02,-245.42 152.04,-245.42"/>
<text xml:space="preserve" text-anchor="start" x="155.04" y="-251.22" font-family="Arial" font-size="14.00" fill="#c9c9c9">draws with its code, builds its packs</text>
</g>
<!-- developer&#45;&gt;loupe -->
<g id="edge1" class="edge">
<title>developer&#45;&gt;loupe</title>
<path fill="none" stroke="#8d8d8d" stroke-width="2" stroke-dasharray="5,2" d="M262.7,-600.4C262.7,-600.4 262.7,-490.45 262.7,-490.45"/>
<polygon fill="#8d8d8d" stroke="#8d8d8d" stroke-width="2" points="265.33,-490.45 262.7,-482.95 260.08,-490.45 265.33,-490.45"/>
<polygon fill="#18191b" fill-opacity="0.627451" stroke="none" points="77.71,-545.42 77.71,-568.22 262.7,-568.22 262.7,-545.42 77.71,-545.42"/>
<text xml:space="preserve" text-anchor="start" x="80.71" y="-551.22" font-family="Arial" font-size="14.00" fill="#c9c9c9">opens scenes, edits, renders</text>
</g>
<!-- ci&#45;&gt;loupe -->
<g id="edge2" class="edge">
<title>ci&#45;&gt;loupe</title>
<path fill="none" stroke="#8d8d8d" stroke-width="2" stroke-dasharray="5,2" d="M491.52,-600.4C491.52,-600.4 491.52,-490.45 491.52,-490.45"/>
<polygon fill="#8d8d8d" stroke="#8d8d8d" stroke-width="2" points="494.15,-490.45 491.52,-482.95 488.9,-490.45 494.15,-490.45"/>
<polygon fill="#18191b" fill-opacity="0.627451" stroke="none" points="350.9,-545.42 350.9,-568.22 491.52,-568.22 491.52,-545.42 350.9,-545.42"/>
<text xml:space="preserve" text-anchor="start" x="353.9" y="-551.22" font-family="Arial" font-size="14.00" fill="#c9c9c9">builds, tests, releases</text>
</g>
</g>
</svg>
`;case`loupe`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="1736pt" height="1227pt"
 viewBox="0.00 0.00 1736.00 1227.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 1212.25)">
<g id="clust1" class="cluster">
<title>cluster_loupe</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-8 8,-1189.2 1698,-1189.2 1698,-8 8,-8"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-1176.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">LOUPE</text>
</g>
<!-- c_path -->
<g id="node1" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="798.02,-1128 477.98,-1128 477.98,-948 798.02,-948 798.02,-1128"/>
<text xml:space="preserve" text-anchor="start" x="618.54" y="-1050.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="587.78" y="-1029.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="526.52" y="-1008.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_storyboard -->
<g id="node2" class="node">
<title>c_storyboard</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1228.02,-1128 907.98,-1128 907.98,-948 1228.02,-948 1228.02,-1128"/>
<text xml:space="preserve" text-anchor="start" x="1020.75" y="-1039.8" font-family="Arial" font-size="20.00" fill="#eff6ff">storyboard</text>
<text xml:space="preserve" text-anchor="start" x="1017.78" y="-1018.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 751 lines</text>
</g>
<!-- c_window -->
<g id="node3" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1109.02,-528 788.98,-528 788.98,-348 1109.02,-348 1109.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="915.65" y="-450.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="891.55" y="-429.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">20 files · 3514 lines</text>
<text xml:space="preserve" text-anchor="start" x="839.6" y="-408.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_editor -->
<g id="node4" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1658.02,-228 1337.98,-228 1337.98,-48 1658.02,-48 1658.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="1472.99" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="1444.16" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 1052 lines</text>
</g>
<!-- c_engine -->
<g id="node5" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="368.02,-228 47.98,-228 47.98,-48 368.02,-48 368.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="177.97" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="157.78" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 994 lines</text>
</g>
<!-- c_log -->
<g id="node6" class="node">
<title>c_log</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1228.02,-228 907.98,-228 907.98,-48 1228.02,-48 1228.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="1054.66" y="-150.8" font-family="Arial" font-size="20.00" fill="#eff6ff">log</text>
<text xml:space="preserve" text-anchor="start" x="1017.78" y="-129.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 611 lines</text>
<text xml:space="preserve" text-anchor="start" x="963.59" y="-108.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe&#39;s log: one sink every line</text>
</g>
<!-- c_update -->
<g id="node7" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="798.02,-228 477.98,-228 477.98,-48 798.02,-48 798.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="607.41" y="-150.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="587.78" y="-129.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 847 lines</text>
<text xml:space="preserve" text-anchor="start" x="535.03" y="-108.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_src -->
<g id="node8" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="798.02,-828 477.98,-828 477.98,-648 798.02,-648 798.02,-828"/>
<text xml:space="preserve" text-anchor="start" x="624.67" y="-739.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="580.55" y="-718.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- c_path&#45;&gt;c_src -->
<g id="edge1" class="edge">
<title>c_path&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M584.66,-948.4C584.66,-948.4 584.66,-838.45 584.66,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="587.29,-838.45 584.66,-830.95 582.04,-838.45 587.29,-838.45"/>
</g>
<!-- c_storyboard&#45;&gt;c_window -->
<g id="edge2" class="edge">
<title>c_storyboard&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1008.5,-948.17C1008.5,-948.17 1008.5,-538.48 1008.5,-538.48"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1011.13,-538.48 1008.5,-530.98 1005.88,-538.48 1011.13,-538.48"/>
</g>
<!-- c_window&#45;&gt;c_editor -->
<g id="edge3" class="edge">
<title>c_window&#45;&gt;c_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1108.9,-438C1253.76,-438 1444.66,-438 1444.66,-438 1444.66,-438 1444.66,-238.13 1444.66,-238.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1447.29,-238.13 1444.66,-230.63 1442.04,-238.13 1447.29,-238.13"/>
</g>
<!-- c_window&#45;&gt;c_engine -->
<g id="edge4" class="edge">
<title>c_window&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M789.08,-438C585.91,-438 261.34,-438 261.34,-438 261.34,-438 261.34,-238.13 261.34,-238.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="263.97,-238.13 261.34,-230.63 258.72,-238.13 263.97,-238.13"/>
</g>
<!-- c_window&#45;&gt;c_log -->
<g id="edge5" class="edge">
<title>c_window&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1008.5,-348.4C1008.5,-348.4 1008.5,-238.45 1008.5,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1011.13,-238.45 1008.5,-230.95 1005.88,-238.45 1011.13,-238.45"/>
</g>
<!-- c_window&#45;&gt;c_update -->
<g id="edge6" class="edge">
<title>c_window&#45;&gt;c_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M793.5,-348.4C793.5,-348.4 793.5,-238.45 793.5,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="796.13,-238.45 793.5,-230.95 790.88,-238.45 796.13,-238.45"/>
</g>
<!-- c_editor&#45;&gt;c_src -->
<g id="edge7" class="edge">
<title>c_editor&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1551.34,-227.83C1551.34,-395.21 1551.34,-738 1551.34,-738 1551.34,-738 808.4,-738 808.4,-738"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="808.4,-735.38 800.9,-738 808.4,-740.63 808.4,-735.38"/>
</g>
<!-- c_engine&#45;&gt;c_src -->
<g id="edge8" class="edge">
<title>c_engine&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M154.66,-227.83C154.66,-395.21 154.66,-738 154.66,-738 154.66,-738 467.53,-738 467.53,-738"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="467.53,-740.63 475.03,-738 467.53,-735.38 467.53,-740.63"/>
</g>
<!-- c_update&#45;&gt;c_src -->
<g id="edge9" class="edge">
<title>c_update&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M633.48,-227.83C633.48,-227.83 633.48,-637.52 633.48,-637.52"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="630.86,-637.52 633.48,-645.02 636.11,-637.52 630.86,-637.52"/>
</g>
<!-- c_src&#45;&gt;c_path -->
<g id="edge10" class="edge">
<title>c_src&#45;&gt;c_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M691.34,-827.6C691.34,-827.6 691.34,-937.55 691.34,-937.55"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="688.72,-937.55 691.34,-945.05 693.97,-937.55 688.72,-937.55"/>
</g>
<!-- c_src&#45;&gt;c_window -->
<g id="edge11" class="edge">
<title>c_src&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M793.5,-648.4C793.5,-648.4 793.5,-538.45 793.5,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="796.13,-538.45 793.5,-530.95 790.88,-538.45 796.13,-538.45"/>
</g>
</g>
</svg>
`;case`loupe_all`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="1660pt" height="2127pt"
 viewBox="0.00 0.00 1660.00 2127.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 2112.25)">
<g id="clust1" class="cluster">
<title>cluster_loupe</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-8 8,-2089.2 1622,-2089.2 1622,-8 8,-8"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-2076.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">LOUPE</text>
</g>
<!-- c_tests -->
<g id="node1" class="node">
<title>c_tests</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1091.02,-2028 770.98,-2028 770.98,-1848 1091.02,-1848 1091.02,-2028"/>
<text xml:space="preserve" text-anchor="start" x="909.88" y="-1939.8" font-family="Arial" font-size="20.00" fill="#eff6ff">tests</text>
<text xml:space="preserve" text-anchor="start" x="888.01" y="-1918.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">0 files · 0 lines</text>
</g>
<!-- c_path -->
<g id="node2" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="744.02,-1728 423.98,-1728 423.98,-1548 744.02,-1548 744.02,-1728"/>
<text xml:space="preserve" text-anchor="start" x="564.54" y="-1650.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="533.78" y="-1629.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="472.52" y="-1608.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_storyboard -->
<g id="node3" class="node">
<title>c_storyboard</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1246.02,-1428 925.98,-1428 925.98,-1248 1246.02,-1248 1246.02,-1428"/>
<text xml:space="preserve" text-anchor="start" x="1038.75" y="-1339.8" font-family="Arial" font-size="20.00" fill="#eff6ff">storyboard</text>
<text xml:space="preserve" text-anchor="start" x="1035.78" y="-1318.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 751 lines</text>
</g>
<!-- c_window -->
<g id="node4" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1157.02,-828 836.98,-828 836.98,-648 1157.02,-648 1157.02,-828"/>
<text xml:space="preserve" text-anchor="start" x="963.65" y="-750.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="939.55" y="-729.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">20 files · 3514 lines</text>
<text xml:space="preserve" text-anchor="start" x="887.6" y="-708.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_editor -->
<g id="node5" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="615.02,-528 294.98,-528 294.98,-348 615.02,-348 615.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="429.99" y="-439.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="401.16" y="-418.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 1052 lines</text>
</g>
<!-- c_update -->
<g id="node6" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1389.02,-528 1068.98,-528 1068.98,-348 1389.02,-348 1389.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="1198.41" y="-450.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="1178.78" y="-429.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 847 lines</text>
<text xml:space="preserve" text-anchor="start" x="1126.03" y="-408.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_engine -->
<g id="node7" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="592.02,-1428 271.98,-1428 271.98,-1248 592.02,-1248 592.02,-1428"/>
<text xml:space="preserve" text-anchor="start" x="401.97" y="-1339.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="381.78" y="-1318.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 994 lines</text>
</g>
<!-- c_src -->
<g id="node8" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="975.02,-1128 654.98,-1128 654.98,-948 975.02,-948 975.02,-1128"/>
<text xml:space="preserve" text-anchor="start" x="801.67" y="-1039.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="757.55" y="-1018.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- c_log -->
<g id="node9" class="node">
<title>c_log</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="982.02,-228 661.98,-228 661.98,-48 982.02,-48 982.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="808.66" y="-150.8" font-family="Arial" font-size="20.00" fill="#eff6ff">log</text>
<text xml:space="preserve" text-anchor="start" x="771.78" y="-129.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 611 lines</text>
<text xml:space="preserve" text-anchor="start" x="717.59" y="-108.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe&#39;s log: one sink every line</text>
</g>
<!-- c_tests&#45;&gt;c_path -->
<g id="edge1" class="edge">
<title>c_tests&#45;&gt;c_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M793.12,-1848.4C793.12,-1760 793.12,-1638 793.12,-1638 793.12,-1638 754.36,-1638 754.36,-1638"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="754.36,-1635.38 746.86,-1638 754.36,-1640.63 754.36,-1635.38"/>
</g>
<!-- c_tests&#45;&gt;c_storyboard -->
<g id="edge2" class="edge">
<title>c_tests&#45;&gt;c_storyboard</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1008.5,-1848.17C1008.5,-1848.17 1008.5,-1438.48 1008.5,-1438.48"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1011.13,-1438.48 1008.5,-1430.98 1005.88,-1438.48 1011.13,-1438.48"/>
</g>
<!-- c_tests&#45;&gt;c_window -->
<g id="edge3" class="edge">
<title>c_tests&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M903.84,-1848.28C903.84,-1649.2 903.84,-1188 903.84,-1188 903.84,-1188 1035.69,-1188 1035.69,-1188 1035.69,-1188 1035.69,-838.51 1035.69,-838.51"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1038.31,-838.51 1035.69,-831.01 1033.06,-838.51 1038.31,-838.51"/>
</g>
<!-- c_tests&#45;&gt;c_editor -->
<g id="edge4" class="edge">
<title>c_tests&#45;&gt;c_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M815.27,-1848.12C815.27,-1722.06 815.27,-1508 815.27,-1508 815.27,-1508 599.69,-1508 599.69,-1508 599.69,-1508 599.69,-538.37 599.69,-538.37"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="602.31,-538.37 599.69,-530.87 597.06,-538.37 602.31,-538.37"/>
</g>
<!-- c_tests&#45;&gt;c_update -->
<g id="edge5" class="edge">
<title>c_tests&#45;&gt;c_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1091,-1938C1197.34,-1938 1317.52,-1938 1317.52,-1938 1317.52,-1938 1317.52,-538.42 1317.52,-538.42"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1320.15,-538.42 1317.52,-530.92 1314.9,-538.42 1320.15,-538.42"/>
</g>
<!-- c_tests&#45;&gt;c_engine -->
<g id="edge6" class="edge">
<title>c_tests&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M859.55,-1848.21C859.55,-1691.14 859.55,-1383 859.55,-1383 859.55,-1383 602.34,-1383 602.34,-1383"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="602.34,-1380.38 594.84,-1383 602.34,-1385.63 602.34,-1380.38"/>
</g>
<!-- c_tests&#45;&gt;c_src -->
<g id="edge7" class="edge">
<title>c_tests&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M881.69,-1848.41C881.69,-1848.41 881.69,-1138.42 881.69,-1138.42"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="884.32,-1138.42 881.69,-1130.92 879.07,-1138.42 884.32,-1138.42"/>
</g>
<!-- c_tests&#45;&gt;c_log -->
<g id="edge8" class="edge">
<title>c_tests&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M837.41,-1848.21C837.41,-1711.79 837.41,-1468 837.41,-1468 837.41,-1468 641.66,-1468 641.66,-1468 641.66,-1468 641.66,-183 641.66,-183 641.66,-183 651.47,-183 651.47,-183"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="651.47,-185.63 658.97,-183 651.47,-180.38 651.47,-185.63"/>
</g>
<!-- c_path&#45;&gt;c_engine -->
<g id="edge9" class="edge">
<title>c_path&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M508,-1548.4C508,-1548.4 508,-1438.45 508,-1438.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="510.63,-1438.45 508,-1430.95 505.38,-1438.45 510.63,-1438.45"/>
</g>
<!-- c_path&#45;&gt;c_src -->
<g id="edge10" class="edge">
<title>c_path&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M708.4,-1548.17C708.4,-1548.17 708.4,-1138.48 708.4,-1138.48"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="711.03,-1138.48 708.4,-1130.98 705.78,-1138.48 711.03,-1138.48"/>
</g>
<!-- c_path&#45;&gt;c_log -->
<g id="edge11" class="edge">
<title>c_path&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M628.34,-1548.1C628.34,-1224.49 628.34,-138 628.34,-138 628.34,-138 651.55,-138 651.55,-138"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="651.55,-140.63 659.05,-138 651.55,-135.38 651.55,-140.63"/>
</g>
<!-- c_storyboard&#45;&gt;c_window -->
<g id="edge12" class="edge">
<title>c_storyboard&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1096.35,-1248.17C1096.35,-1248.17 1096.35,-838.48 1096.35,-838.48"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1098.98,-838.48 1096.35,-830.98 1093.73,-838.48 1098.98,-838.48"/>
</g>
<!-- c_storyboard&#45;&gt;c_src -->
<g id="edge13" class="edge">
<title>c_storyboard&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M950.5,-1248.4C950.5,-1248.4 950.5,-1138.45 950.5,-1138.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="953.13,-1138.45 950.5,-1130.95 947.88,-1138.45 953.13,-1138.45"/>
</g>
<!-- c_window&#45;&gt;c_editor -->
<g id="edge14" class="edge">
<title>c_window&#45;&gt;c_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M837.11,-708C729.57,-708 607.35,-708 607.35,-708 607.35,-708 607.35,-538.43 607.35,-538.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="609.98,-538.43 607.35,-530.93 604.73,-538.43 609.98,-538.43"/>
</g>
<!-- c_window&#45;&gt;c_update -->
<g id="edge15" class="edge">
<title>c_window&#45;&gt;c_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1113,-648.4C1113,-648.4 1113,-538.45 1113,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1115.63,-538.45 1113,-530.95 1110.38,-538.45 1115.63,-538.45"/>
</g>
<!-- c_window&#45;&gt;c_engine -->
<g id="edge16" class="edge">
<title>c_window&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M837.19,-768C689.57,-768 493.01,-768 493.01,-768 493.01,-768 493.01,-1237.78 493.01,-1237.78"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="490.38,-1237.78 493.01,-1245.28 495.63,-1237.78 490.38,-1237.78"/>
</g>
<!-- c_window&#45;&gt;c_src -->
<g id="edge17" class="edge">
<title>c_window&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M882.99,-827.6C882.99,-827.6 882.99,-937.55 882.99,-937.55"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="880.37,-937.55 882.99,-945.05 885.62,-937.55 880.37,-937.55"/>
</g>
<!-- c_window&#45;&gt;c_log -->
<g id="edge18" class="edge">
<title>c_window&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M885.33,-648.17C885.33,-648.17 885.33,-238.48 885.33,-238.48"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="887.95,-238.48 885.33,-230.98 882.7,-238.48 887.95,-238.48"/>
</g>
<!-- c_editor&#45;&gt;c_engine -->
<g id="edge19" class="edge">
<title>c_editor&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M393.99,-527.59C393.99,-527.59 393.99,-1237.58 393.99,-1237.58"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="391.37,-1237.58 393.99,-1245.08 396.62,-1237.58 391.37,-1237.58"/>
</g>
<!-- c_editor&#45;&gt;c_src -->
<g id="edge20" class="edge">
<title>c_editor&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M614.83,-468C640.31,-468 658.48,-468 658.48,-468 658.48,-468 658.48,-937.78 658.48,-937.78"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="655.86,-937.78 658.48,-945.28 661.11,-937.78 655.86,-937.78"/>
</g>
<!-- c_update&#45;&gt;c_src -->
<g id="edge21" class="edge">
<title>c_update&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1201.52,-527.83C1201.52,-695.21 1201.52,-1038 1201.52,-1038 1201.52,-1038 985.53,-1038 985.53,-1038"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="985.53,-1035.38 978.03,-1038 985.53,-1040.63 985.53,-1035.38"/>
</g>
<!-- c_update&#45;&gt;c_log -->
<g id="edge22" class="edge">
<title>c_update&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1069.24,-408C999.65,-408 933.67,-408 933.67,-408 933.67,-408 933.67,-238.43 933.67,-238.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="936.3,-238.43 933.67,-230.93 931.05,-238.43 936.3,-238.43"/>
</g>
<!-- c_engine&#45;&gt;c_src -->
<g id="edge23" class="edge">
<title>c_engine&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M591.94,-1293C636.41,-1293 672.79,-1293 672.79,-1293 672.79,-1293 672.79,-1138.43 672.79,-1138.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="675.41,-1138.43 672.79,-1130.93 670.16,-1138.43 675.41,-1138.43"/>
</g>
<!-- c_engine&#45;&gt;c_log -->
<g id="edge24" class="edge">
<title>c_engine&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M283.48,-1248.21C283.48,-962.49 283.48,-93 283.48,-93 283.48,-93 651.72,-93 651.72,-93"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="651.72,-95.63 659.22,-93 651.72,-90.38 651.72,-95.63"/>
</g>
<!-- c_src&#45;&gt;c_path -->
<g id="edge25" class="edge">
<title>c_src&#45;&gt;c_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M726.21,-1127.83C726.21,-1127.83 726.21,-1537.52 726.21,-1537.52"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="723.59,-1537.52 726.21,-1545.02 728.84,-1537.52 723.59,-1537.52"/>
</g>
<!-- c_src&#45;&gt;c_window -->
<g id="edge26" class="edge">
<title>c_src&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M929.01,-948.4C929.01,-948.4 929.01,-838.45 929.01,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="931.63,-838.45 929.01,-830.95 926.38,-838.45 931.63,-838.45"/>
</g>
<!-- c_src&#45;&gt;c_engine -->
<g id="edge27" class="edge">
<title>c_src&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M690.6,-1127.6C690.6,-1216 690.6,-1338 690.6,-1338 690.6,-1338 602.25,-1338 602.25,-1338"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="602.25,-1335.38 594.75,-1338 602.25,-1340.63 602.25,-1335.38"/>
</g>
<!-- c_src&#45;&gt;c_log -->
<g id="edge28" class="edge">
<title>c_src&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M749.48,-948.41C749.48,-948.41 749.48,-238.42 749.48,-238.42"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="752.11,-238.42 749.48,-230.92 746.86,-238.42 752.11,-238.42"/>
</g>
</g>
</svg>
`;case`c_editor`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="940pt" height="1710pt"
 viewBox="0.00 0.00 940.00 1710.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 1695.05)">
<g id="clust1" class="cluster">
<title>cluster_c_editor</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-260 8,-1441.2 520,-1441.2 520,-260 8,-260"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-1428.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">EDITOR</text>
</g>
<!-- m_scene_tool -->
<g id="node1" class="node">
<title>m_scene_tool</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="480.02,-1380 159.98,-1380 159.98,-1200 480.02,-1200 480.02,-1380"/>
<text xml:space="preserve" text-anchor="start" x="271.63" y="-1302.8" font-family="Arial" font-size="20.00" fill="#f8fafc">scene_tool</text>
<text xml:space="preserve" text-anchor="start" x="228.57" y="-1281.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">scene_tool.cpp, scene_tool.hpp</text>
<text xml:space="preserve" text-anchor="start" x="219.58" y="-1260.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The editor&#39;s calls into autana&#39;s</text>
</g>
<!-- m_doc -->
<g id="node2" class="node">
<title>m_doc</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="480.02,-1080 159.98,-1080 159.98,-900 480.02,-900 480.02,-1080"/>
<text xml:space="preserve" text-anchor="start" x="303.88" y="-1002.8" font-family="Arial" font-size="20.00" fill="#f8fafc">doc</text>
<text xml:space="preserve" text-anchor="start" x="270.49" y="-981.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">doc.cpp, doc.hpp</text>
<text xml:space="preserve" text-anchor="start" x="210.77" y="-960.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">What the editor holds and saves:</text>
</g>
<!-- m_camera_path -->
<g id="node3" class="node">
<title>m_camera_path</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-780 47.98,-780 47.98,-600 368.02,-600 368.02,-780"/>
<text xml:space="preserve" text-anchor="start" x="149.63" y="-702.8" font-family="Arial" font-size="20.00" fill="#f8fafc">camera_path</text>
<text xml:space="preserve" text-anchor="start" x="103.57" y="-681.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">camera_path.cpp, camera_path.hpp</text>
<text xml:space="preserve" text-anchor="start" x="103.79" y="-660.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A camera path as its .keys.toml</text>
</g>
<!-- m_placement -->
<g id="node4" class="node">
<title>m_placement</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="424.02,-480 103.98,-480 103.98,-300 424.02,-300 424.02,-480"/>
<text xml:space="preserve" text-anchor="start" x="217.86" y="-402.8" font-family="Arial" font-size="20.00" fill="#f8fafc">placement</text>
<text xml:space="preserve" text-anchor="start" x="175.47" y="-381.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">placement.cpp, placement.hpp</text>
<text xml:space="preserve" text-anchor="start" x="152.27" y="-360.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A Placement both ways: to the en</text>
</g>
<!-- c_window -->
<g id="node5" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="480.02,-1680 159.98,-1680 159.98,-1500 480.02,-1500 480.02,-1680"/>
<text xml:space="preserve" text-anchor="start" x="286.65" y="-1602.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="262.55" y="-1581.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">20 files · 3514 lines</text>
<text xml:space="preserve" text-anchor="start" x="210.6" y="-1560.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_src -->
<g id="node6" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="910.02,-1080 589.98,-1080 589.98,-900 910.02,-900 910.02,-1080"/>
<text xml:space="preserve" text-anchor="start" x="736.67" y="-991.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="692.55" y="-970.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- c_engine -->
<g id="node7" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="424.02,-180 103.98,-180 103.98,0 424.02,0 424.02,-180"/>
<text xml:space="preserve" text-anchor="start" x="233.97" y="-91.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="213.78" y="-70.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 994 lines</text>
</g>
<!-- m_scene_tool&#45;&gt;m_doc -->
<g id="edge2" class="edge">
<title>m_scene_tool&#45;&gt;m_doc</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M320,-1200.4C320,-1200.4 320,-1090.45 320,-1090.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="322.63,-1090.45 320,-1082.95 317.38,-1090.45 322.63,-1090.45"/>
</g>
<!-- m_scene_tool&#45;&gt;c_src -->
<g id="edge3" class="edge">
<title>m_scene_tool&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M479.81,-1290C602.13,-1290 750,-1290 750,-1290 750,-1290 750,-1090.13 750,-1090.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="752.63,-1090.13 750,-1082.63 747.38,-1090.13 752.63,-1090.13"/>
</g>
<!-- m_doc&#45;&gt;m_camera_path -->
<g id="edge4" class="edge">
<title>m_doc&#45;&gt;m_camera_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M264,-900.4C264,-900.4 264,-790.45 264,-790.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="266.63,-790.45 264,-782.95 261.38,-790.45 266.63,-790.45"/>
</g>
<!-- m_camera_path&#45;&gt;m_placement -->
<g id="edge5" class="edge">
<title>m_camera_path&#45;&gt;m_placement</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M236,-600.4C236,-600.4 236,-490.45 236,-490.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="238.63,-490.45 236,-482.95 233.38,-490.45 238.63,-490.45"/>
</g>
<!-- m_placement&#45;&gt;m_doc -->
<g id="edge6" class="edge">
<title>m_placement&#45;&gt;m_doc</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M396.02,-479.83C396.02,-479.83 396.02,-889.52 396.02,-889.52"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="393.4,-889.52 396.02,-897.02 398.65,-889.52 393.4,-889.52"/>
</g>
<!-- m_placement&#45;&gt;c_engine -->
<g id="edge7" class="edge">
<title>m_placement&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M264,-300.4C264,-300.4 264,-190.45 264,-190.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="266.63,-190.45 264,-182.95 261.38,-190.45 266.63,-190.45"/>
</g>
<!-- c_window&#45;&gt;m_scene_tool -->
<g id="edge1" class="edge">
<title>c_window&#45;&gt;m_scene_tool</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M320,-1500.4C320,-1500.4 320,-1390.45 320,-1390.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="322.63,-1390.45 320,-1382.95 317.38,-1390.45 322.63,-1390.45"/>
</g>
</g>
</svg>
`;case`c_engine`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="2040pt" height="858pt"
 viewBox="0.00 0.00 2040.00 858.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 843.05)">
<g id="clust1" class="cluster">
<title>cluster_c_engine</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="390.02,-8 390.02,-589.2 1650.02,-589.2 1650.02,-8 390.02,-8"/>
<text xml:space="preserve" text-anchor="start" x="398.02" y="-576.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">ENGINE</text>
</g>
<!-- m_frames -->
<g id="node1" class="node">
<title>m_frames</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1610.04,-528 1290,-528 1290,-348 1610.04,-348 1610.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="1419.46" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">frames</text>
<text xml:space="preserve" text-anchor="start" x="1381.74" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">frames.cpp, frames.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1344.39" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Gfx&#39;s frame from a provider, dra</text>
</g>
<!-- m_path_curve -->
<g id="node2" class="node">
<title>m_path_curve</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="750.04,-528 430,-528 430,-348 750.04,-348 750.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="540.54" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">path_curve</text>
<text xml:space="preserve" text-anchor="start" x="497.15" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">path_curve.cpp, path_curve.hpp</text>
<text xml:space="preserve" text-anchor="start" x="480.37" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A camera path as the clip autana</text>
</g>
<!-- m_engine_frame -->
<g id="node3" class="node">
<title>m_engine_frame</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1180.04,-528 860,-528 860,-348 1180.04,-348 1180.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="958.87" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">engine_frame</text>
<text xml:space="preserve" text-anchor="start" x="911.97" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">engine_frame.cpp, engine_frame.hpp</text>
<text xml:space="preserve" text-anchor="start" x="912.29" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The z mirror between autana&#39;s fr</text>
</g>
<!-- m_autana -->
<g id="node4" class="node">
<title>m_autana</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="750.04,-228 430,-228 430,-48 750.04,-48 750.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="559.43" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">autana</text>
<text xml:space="preserve" text-anchor="start" x="557.49" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">autana.hpp</text>
<text xml:space="preserve" text-anchor="start" x="477.44" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The one place Loupe includes aut</text>
</g>
<!-- m_pose -->
<g id="node5" class="node">
<title>m_pose</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1180.04,-228 860,-228 860,-48 1180.04,-48 1180.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="998.34" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">pose</text>
<text xml:space="preserve" text-anchor="start" x="993.27" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">pose.hpp</text>
<text xml:space="preserve" text-anchor="start" x="905.6" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Autana&#39;s packages (launcher/pack</text>
</g>
<!-- c_editor -->
<g id="node6" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="750.04,-828 430,-828 430,-648 750.04,-648 750.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="565.01" y="-739.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="536.18" y="-718.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 1052 lines</text>
</g>
<!-- c_path -->
<g id="node7" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1610.04,-828 1290,-828 1290,-648 1610.04,-648 1610.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="1430.56" y="-750.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="1399.8" y="-729.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="1338.54" y="-708.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_src -->
<g id="node8" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-528 0,-528 0,-348 320.04,-348 320.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="146.69" y="-439.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="102.57" y="-418.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- c_window -->
<g id="node9" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1180.04,-828 860,-828 860,-648 1180.04,-648 1180.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="986.67" y="-750.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="962.57" y="-729.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">20 files · 3514 lines</text>
<text xml:space="preserve" text-anchor="start" x="910.62" y="-708.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_log -->
<g id="node10" class="node">
<title>c_log</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2010.04,-228 1690,-228 1690,-48 2010.04,-48 2010.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="1836.68" y="-150.8" font-family="Arial" font-size="20.00" fill="#eff6ff">log</text>
<text xml:space="preserve" text-anchor="start" x="1799.8" y="-129.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 611 lines</text>
<text xml:space="preserve" text-anchor="start" x="1745.61" y="-108.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe&#39;s log: one sink every line</text>
</g>
<!-- m_frames&#45;&gt;m_pose -->
<g id="edge10" class="edge">
<title>m_frames&#45;&gt;m_pose</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1396.68,-348.09C1396.68,-269.55 1396.68,-168 1396.68,-168 1396.68,-168 1190.56,-168 1190.56,-168"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1190.56,-165.38 1183.06,-168 1190.56,-170.63 1190.56,-165.38"/>
</g>
<!-- m_frames&#45;&gt;c_src -->
<g id="edge8" class="edge">
<title>m_frames&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1418.02,-527.85C1418.02,-559.8 1418.02,-588 1418.02,-588 1418.02,-588 213.36,-588 213.36,-588 213.36,-588 213.36,-538.38 213.36,-538.38"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="215.99,-538.38 213.36,-530.88 210.74,-538.38 215.99,-538.38"/>
</g>
<!-- m_frames&#45;&gt;c_log -->
<g id="edge9" class="edge">
<title>m_frames&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1503.36,-348.26C1503.36,-250.54 1503.36,-108 1503.36,-108 1503.36,-108 1679.61,-108 1679.61,-108"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1679.61,-110.63 1687.11,-108 1679.61,-105.38 1679.61,-110.63"/>
</g>
<!-- m_path_curve&#45;&gt;m_autana -->
<g id="edge11" class="edge">
<title>m_path_curve&#45;&gt;m_autana</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M590.02,-348.4C590.02,-348.4 590.02,-238.45 590.02,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="592.65,-238.45 590.02,-230.95 587.4,-238.45 592.65,-238.45"/>
</g>
<!-- m_path_curve&#45;&gt;m_pose -->
<g id="edge12" class="edge">
<title>m_path_curve&#45;&gt;m_pose</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M749.93,-438C781.46,-438 805.02,-438 805.02,-438 805.02,-438 805.02,-138 805.02,-138 805.02,-138 849.58,-138 849.58,-138"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="849.58,-140.63 857.08,-138 849.58,-135.38 849.58,-140.63"/>
</g>
<!-- m_engine_frame&#45;&gt;m_pose -->
<g id="edge13" class="edge">
<title>m_engine_frame&#45;&gt;m_pose</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1020.02,-348.4C1020.02,-348.4 1020.02,-238.45 1020.02,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1022.65,-238.45 1020.02,-230.95 1017.4,-238.45 1022.65,-238.45"/>
</g>
<!-- c_editor&#45;&gt;m_frames -->
<g id="edge1" class="edge">
<title>c_editor&#45;&gt;m_frames</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M643.36,-648.28C643.36,-630.92 643.36,-618 643.36,-618 643.36,-618 1482.02,-618 1482.02,-618 1482.02,-618 1482.02,-538.53 1482.02,-538.53"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1484.65,-538.53 1482.02,-531.03 1479.4,-538.53 1484.65,-538.53"/>
</g>
<!-- c_editor&#45;&gt;m_path_curve -->
<g id="edge2" class="edge">
<title>c_editor&#45;&gt;m_path_curve</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M536.68,-648.4C536.68,-648.4 536.68,-538.45 536.68,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="539.31,-538.45 536.68,-530.95 534.06,-538.45 539.31,-538.45"/>
</g>
<!-- c_path&#45;&gt;m_frames -->
<g id="edge3" class="edge">
<title>c_path&#45;&gt;m_frames</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1546.03,-648.4C1546.03,-648.4 1546.03,-538.45 1546.03,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1548.66,-538.45 1546.03,-530.95 1543.41,-538.45 1548.66,-538.45"/>
</g>
<!-- c_src&#45;&gt;m_frames -->
<g id="edge4" class="edge">
<title>c_src&#45;&gt;m_frames</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M106.68,-527.72C106.68,-545.08 106.68,-558 106.68,-558 106.68,-558 1354.01,-558 1354.01,-558 1354.01,-558 1354.01,-538.25 1354.01,-538.25"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1356.63,-538.25 1354.01,-530.75 1351.38,-538.25 1356.63,-538.25"/>
</g>
<!-- c_src&#45;&gt;m_autana -->
<g id="edge5" class="edge">
<title>c_src&#45;&gt;m_autana</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M160.02,-348.4C160.02,-260 160.02,-138 160.02,-138 160.02,-138 419.68,-138 419.68,-138"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="419.68,-140.63 427.18,-138 419.68,-135.38 419.68,-140.63"/>
</g>
<!-- c_window&#45;&gt;m_frames -->
<g id="edge6" class="edge">
<title>c_window&#45;&gt;m_frames</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1179.93,-738C1211.46,-738 1235.02,-738 1235.02,-738 1235.02,-738 1235.02,-438 1235.02,-438 1235.02,-438 1279.58,-438 1279.58,-438"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1279.58,-440.63 1287.08,-438 1279.58,-435.38 1279.58,-440.63"/>
</g>
<!-- c_window&#45;&gt;m_engine_frame -->
<g id="edge7" class="edge">
<title>c_window&#45;&gt;m_engine_frame</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1020.02,-648.4C1020.02,-648.4 1020.02,-538.45 1020.02,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1022.65,-538.45 1020.02,-530.95 1017.4,-538.45 1022.65,-538.45"/>
</g>
</g>
</svg>
`;case`c_log`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="2070pt" height="550pt"
 viewBox="0.00 0.00 2070.00 550.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 535.05)">
<g id="clust1" class="cluster">
<title>cluster_c_log</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="828.02,-8 828.02,-273.2 1212.02,-273.2 1212.02,-8 828.02,-8"/>
<text xml:space="preserve" text-anchor="start" x="836.02" y="-260.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">LOG</text>
</g>
<!-- m_log -->
<g id="node1" class="node">
<title>m_log</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1180.04,-220 860,-220 860,-40 1180.04,-40 1180.04,-220"/>
<text xml:space="preserve" text-anchor="start" x="1006.68" y="-142.8" font-family="Arial" font-size="20.00" fill="#f8fafc">log</text>
<text xml:space="preserve" text-anchor="start" x="974.12" y="-121.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">log.cpp, log.hpp</text>
<text xml:space="preserve" text-anchor="start" x="915.61" y="-100.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Loupe&#39;s log: one sink every line</text>
</g>
<!-- c_engine -->
<g id="node2" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-520 0,-520 0,-340 320.04,-340 320.04,-520"/>
<text xml:space="preserve" text-anchor="start" x="129.99" y="-431.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="109.8" y="-410.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 994 lines</text>
</g>
<!-- c_path -->
<g id="node3" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="750.04,-520 430,-520 430,-340 750.04,-340 750.04,-520"/>
<text xml:space="preserve" text-anchor="start" x="570.56" y="-442.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="539.8" y="-421.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="478.54" y="-400.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_src -->
<g id="node4" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1180.04,-520 860,-520 860,-340 1180.04,-340 1180.04,-520"/>
<text xml:space="preserve" text-anchor="start" x="1006.69" y="-431.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="962.57" y="-410.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- c_update -->
<g id="node5" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1610.04,-520 1290,-520 1290,-340 1610.04,-340 1610.04,-520"/>
<text xml:space="preserve" text-anchor="start" x="1419.43" y="-442.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="1399.8" y="-421.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 847 lines</text>
<text xml:space="preserve" text-anchor="start" x="1347.05" y="-400.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_window -->
<g id="node6" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2040.04,-520 1720,-520 1720,-340 2040.04,-340 2040.04,-520"/>
<text xml:space="preserve" text-anchor="start" x="1846.67" y="-442.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="1822.57" y="-421.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">20 files · 3514 lines</text>
<text xml:space="preserve" text-anchor="start" x="1770.62" y="-400.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_engine&#45;&gt;m_log -->
<g id="edge1" class="edge">
<title>c_engine&#45;&gt;m_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M160.02,-340.26C160.02,-242.54 160.02,-100 160.02,-100 160.02,-100 849.5,-100 849.5,-100"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="849.5,-102.63 857,-100 849.5,-97.38 849.5,-102.63"/>
</g>
<!-- c_path&#45;&gt;m_log -->
<g id="edge2" class="edge">
<title>c_path&#45;&gt;m_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M590.02,-340.09C590.02,-261.55 590.02,-160 590.02,-160 590.02,-160 849.68,-160 849.68,-160"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="849.68,-162.63 857.18,-160 849.68,-157.38 849.68,-162.63"/>
</g>
<!-- c_src&#45;&gt;m_log -->
<g id="edge3" class="edge">
<title>c_src&#45;&gt;m_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1020.02,-340.4C1020.02,-340.4 1020.02,-230.45 1020.02,-230.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1022.65,-230.45 1020.02,-222.95 1017.4,-230.45 1022.65,-230.45"/>
</g>
<!-- c_update&#45;&gt;m_log -->
<g id="edge4" class="edge">
<title>c_update&#45;&gt;m_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1450.02,-340.09C1450.02,-261.55 1450.02,-160 1450.02,-160 1450.02,-160 1190.36,-160 1190.36,-160"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1190.36,-157.38 1182.86,-160 1190.36,-162.63 1190.36,-157.38"/>
</g>
<!-- c_window&#45;&gt;m_log -->
<g id="edge5" class="edge">
<title>c_window&#45;&gt;m_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1880.02,-340.26C1880.02,-242.54 1880.02,-100 1880.02,-100 1880.02,-100 1190.54,-100 1190.54,-100"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1190.54,-97.38 1183.04,-100 1190.54,-102.63 1190.54,-97.38"/>
</g>
</g>
</svg>
`;case`c_path`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="1210pt" height="571pt"
 viewBox="0.00 0.00 1210.00 571.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 556.25)">
<g id="clust1" class="cluster">
<title>cluster_c_path</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="398.02,-268 398.02,-533.2 782.02,-533.2 782.02,-268 398.02,-268"/>
<text xml:space="preserve" text-anchor="start" x="406.02" y="-520.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">PATH</text>
</g>
<!-- m_path -->
<g id="node1" class="node">
<title>m_path</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="750.04,-480 430,-480 430,-300 750.04,-300 750.04,-480"/>
<text xml:space="preserve" text-anchor="start" x="570.56" y="-402.8" font-family="Arial" font-size="20.00" fill="#f8fafc">path</text>
<text xml:space="preserve" text-anchor="start" x="536.17" y="-381.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">path.cpp, path.hpp</text>
<text xml:space="preserve" text-anchor="start" x="478.54" y="-360.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A scene&#39;s camera path from the c</text>
</g>
<!-- c_src -->
<g id="node2" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-180 0,-180 0,0 320.04,0 320.04,-180"/>
<text xml:space="preserve" text-anchor="start" x="146.69" y="-91.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="102.57" y="-70.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- c_engine -->
<g id="node3" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="750.04,-180 430,-180 430,0 750.04,0 750.04,-180"/>
<text xml:space="preserve" text-anchor="start" x="559.99" y="-91.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="539.8" y="-70.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 994 lines</text>
</g>
<!-- c_log -->
<g id="node4" class="node">
<title>c_log</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1180.04,-180 860,-180 860,0 1180.04,0 1180.04,-180"/>
<text xml:space="preserve" text-anchor="start" x="1006.68" y="-102.8" font-family="Arial" font-size="20.00" fill="#eff6ff">log</text>
<text xml:space="preserve" text-anchor="start" x="969.8" y="-81.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 611 lines</text>
<text xml:space="preserve" text-anchor="start" x="915.61" y="-60.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe&#39;s log: one sink every line</text>
</g>
<!-- m_path&#45;&gt;c_src -->
<g id="edge2" class="edge">
<title>m_path&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M430.08,-420C289.34,-420 106.68,-420 106.68,-420 106.68,-420 106.68,-190.27 106.68,-190.27"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="109.31,-190.27 106.68,-182.77 104.06,-190.27 109.31,-190.27"/>
</g>
<!-- m_path&#45;&gt;c_engine -->
<g id="edge3" class="edge">
<title>m_path&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M590.02,-300.4C590.02,-300.4 590.02,-190.45 590.02,-190.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="592.65,-190.45 590.02,-182.95 587.4,-190.45 592.65,-190.45"/>
</g>
<!-- m_path&#45;&gt;c_log -->
<g id="edge4" class="edge">
<title>m_path&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M749.83,-390C872.15,-390 1020.02,-390 1020.02,-390 1020.02,-390 1020.02,-190.13 1020.02,-190.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1022.65,-190.13 1020.02,-182.63 1017.4,-190.13 1022.65,-190.13"/>
</g>
<!-- c_src&#45;&gt;m_path -->
<g id="edge1" class="edge">
<title>c_src&#45;&gt;m_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M213.36,-179.91C213.36,-258.45 213.36,-360 213.36,-360 213.36,-360 419.48,-360 419.48,-360"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="419.48,-362.63 426.98,-360 419.48,-357.38 419.48,-362.63"/>
</g>
</g>
</svg>
`;case`c_src`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="2900pt" height="1758pt"
 viewBox="0.00 0.00 2900.00 1758.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 1743.05)">
<g id="clust1" class="cluster">
<title>cluster_c_src</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="390.02,-8 390.02,-1489.2 2510.02,-1489.2 2510.02,-8 390.02,-8"/>
<text xml:space="preserve" text-anchor="start" x="398.02" y="-1476.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">SRC</text>
</g>
<!-- m_main -->
<g id="node1" class="node">
<title>m_main</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="750.04,-1428 430,-1428 430,-1248 750.04,-1248 750.04,-1428"/>
<text xml:space="preserve" text-anchor="start" x="568.35" y="-1350.8" font-family="Arial" font-size="20.00" fill="#f8fafc">main</text>
<text xml:space="preserve" text-anchor="start" x="563.65" y="-1329.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">main.cpp</text>
<text xml:space="preserve" text-anchor="start" x="481.21" y="-1308.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Opens a scene in the window, or</text>
</g>
<!-- m_options -->
<g id="node2" class="node">
<title>m_options</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="2470.04,-1428 2150,-1428 2150,-1248 2470.04,-1248 2470.04,-1428"/>
<text xml:space="preserve" text-anchor="start" x="2277.77" y="-1350.8" font-family="Arial" font-size="20.00" fill="#f8fafc">options</text>
<text xml:space="preserve" text-anchor="start" x="2239.55" y="-1329.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">options.cpp, options.hpp</text>
<text xml:space="preserve" text-anchor="start" x="2199.54" y="-1308.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">What one Loupe run is asked for:</text>
</g>
<!-- m_scratch -->
<g id="node3" class="node">
<title>m_scratch</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1610.04,-1428 1290,-1428 1290,-1248 1610.04,-1248 1610.04,-1428"/>
<text xml:space="preserve" text-anchor="start" x="1417.79" y="-1350.8" font-family="Arial" font-size="20.00" fill="#f8fafc">scratch</text>
<text xml:space="preserve" text-anchor="start" x="1379.57" y="-1329.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">scratch.cpp, scratch.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1351.87" y="-1308.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A folder of a run&#39;s own files, u</text>
</g>
<!-- m_version -->
<g id="node4" class="node">
<title>m_version</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="2040.04,-1428 1720,-1428 1720,-1248 2040.04,-1248 2040.04,-1428"/>
<text xml:space="preserve" text-anchor="start" x="1847.78" y="-1350.8" font-family="Arial" font-size="20.00" fill="#f8fafc">version</text>
<text xml:space="preserve" text-anchor="start" x="1809.56" y="-1329.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">version.cpp, version.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1777.06" y="-1308.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Which Loupe this is: its version</text>
</g>
<!-- m_library -->
<g id="node5" class="node">
<title>m_library</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1180.04,-1428 860,-1428 860,-1248 1180.04,-1248 1180.04,-1428"/>
<text xml:space="preserve" text-anchor="start" x="992.79" y="-1350.8" font-family="Arial" font-size="20.00" fill="#f8fafc">library</text>
<text xml:space="preserve" text-anchor="start" x="956.08" y="-1329.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">library.cpp, library.hpp</text>
<text xml:space="preserve" text-anchor="start" x="909.37" y="-1308.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The scenes the window&#39;s start pa</text>
</g>
<!-- m_pack -->
<g id="node6" class="node">
<title>m_pack</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1662.04,-1128 1342,-1128 1342,-948 1662.04,-948 1662.04,-1128"/>
<text xml:space="preserve" text-anchor="start" x="1480.9" y="-1050.8" font-family="Arial" font-size="20.00" fill="#f8fafc">pack</text>
<text xml:space="preserve" text-anchor="start" x="1446.01" y="-1029.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">pack.cpp, pack.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1393.03" y="-1008.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A scene&#39;s asset pack: a .apak as</text>
</g>
<!-- m_tools -->
<g id="node7" class="node">
<title>m_tools</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1662.04,-828 1342,-828 1342,-648 1662.04,-648 1662.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="1480.9" y="-750.8" font-family="Arial" font-size="20.00" fill="#f8fafc">tools</text>
<text xml:space="preserve" text-anchor="start" x="1446.01" y="-729.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">tools.cpp, tools.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1392.79" y="-708.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The programs a build with the pa</text>
</g>
<!-- m_process -->
<g id="node8" class="node">
<title>m_process</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1662.04,-528 1342,-528 1342,-348 1662.04,-348 1662.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="1467.01" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">process</text>
<text xml:space="preserve" text-anchor="start" x="1427.95" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">process.cpp, process.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1394.04" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Other processes: the tools loupe</text>
</g>
<!-- m_paths -->
<g id="node9" class="node">
<title>m_paths</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1662.04,-228 1342,-228 1342,-48 1662.04,-48 1662.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="1477.56" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">paths</text>
<text xml:space="preserve" text-anchor="start" x="1441.67" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">paths.cpp, paths.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1394.05" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A path as UTF&#45;8 text and back: t</text>
</g>
<!-- c_editor -->
<g id="node10" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="350.04,-1128 30,-1128 30,-948 350.04,-948 350.04,-1128"/>
<text xml:space="preserve" text-anchor="start" x="165.01" y="-1039.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="136.18" y="-1018.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 1052 lines</text>
</g>
<!-- c_engine -->
<g id="node11" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2870.04,-828 2550,-828 2550,-648 2870.04,-648 2870.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="2679.99" y="-739.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="2659.8" y="-718.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 994 lines</text>
</g>
<!-- c_storyboard -->
<g id="node12" class="node">
<title>c_storyboard</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2470.04,-1728 2150,-1728 2150,-1548 2470.04,-1548 2470.04,-1728"/>
<text xml:space="preserve" text-anchor="start" x="2262.77" y="-1639.8" font-family="Arial" font-size="20.00" fill="#eff6ff">storyboard</text>
<text xml:space="preserve" text-anchor="start" x="2259.8" y="-1618.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 751 lines</text>
</g>
<!-- c_update -->
<g id="node13" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2040.04,-1728 1720,-1728 1720,-1548 2040.04,-1548 2040.04,-1728"/>
<text xml:space="preserve" text-anchor="start" x="1849.43" y="-1650.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="1829.8" y="-1629.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 847 lines</text>
<text xml:space="preserve" text-anchor="start" x="1777.05" y="-1608.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_path -->
<g id="node14" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-1428 0,-1428 0,-1248 320.04,-1248 320.04,-1428"/>
<text xml:space="preserve" text-anchor="start" x="140.56" y="-1350.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="109.8" y="-1329.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="48.54" y="-1308.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_window -->
<g id="node15" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1395.04,-1728 1075,-1728 1075,-1548 1395.04,-1548 1395.04,-1728"/>
<text xml:space="preserve" text-anchor="start" x="1201.67" y="-1650.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="1177.57" y="-1629.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">20 files · 3514 lines</text>
<text xml:space="preserve" text-anchor="start" x="1125.62" y="-1608.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_log -->
<g id="node16" class="node">
<title>c_log</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2870.04,-228 2550,-228 2550,-48 2870.04,-48 2870.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="2696.68" y="-150.8" font-family="Arial" font-size="20.00" fill="#eff6ff">log</text>
<text xml:space="preserve" text-anchor="start" x="2659.8" y="-129.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 611 lines</text>
<text xml:space="preserve" text-anchor="start" x="2605.61" y="-108.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe&#39;s log: one sink every line</text>
</g>
<!-- m_main&#45;&gt;c_path -->
<g id="edge7" class="edge">
<title>m_main&#45;&gt;c_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M430.21,-1368C430.21,-1368 330.3,-1368 330.3,-1368"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="330.3,-1365.38 322.8,-1368 330.3,-1370.63 330.3,-1365.38"/>
</g>
<!-- m_main&#45;&gt;c_window -->
<g id="edge8" class="edge">
<title>m_main&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M590.02,-1427.6C590.02,-1516 590.02,-1638 590.02,-1638 590.02,-1638 1064.64,-1638 1064.64,-1638"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1064.64,-1640.63 1072.14,-1638 1064.64,-1635.38 1064.64,-1640.63"/>
</g>
<!-- m_options&#45;&gt;m_pack -->
<g id="edge14" class="edge">
<title>m_options&#45;&gt;m_pack</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2310.02,-1248.09C2310.02,-1169.55 2310.02,-1068 2310.02,-1068 2310.02,-1068 1672.18,-1068 1672.18,-1068"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1672.18,-1065.38 1664.68,-1068 1672.18,-1070.63 1672.18,-1065.38"/>
</g>
<!-- m_library&#45;&gt;m_pack -->
<g id="edge15" class="edge">
<title>m_library&#45;&gt;m_pack</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1020.02,-1248.09C1020.02,-1169.55 1020.02,-1068 1020.02,-1068 1020.02,-1068 1331.66,-1068 1331.66,-1068"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1331.66,-1070.63 1339.16,-1068 1331.66,-1065.38 1331.66,-1070.63"/>
</g>
<!-- m_pack&#45;&gt;m_tools -->
<g id="edge17" class="edge">
<title>m_pack&#45;&gt;m_tools</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1502.02,-948.4C1502.02,-948.4 1502.02,-838.45 1502.02,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1504.65,-838.45 1502.02,-830.95 1499.4,-838.45 1504.65,-838.45"/>
</g>
<!-- m_pack&#45;&gt;c_engine -->
<g id="edge16" class="edge">
<title>m_pack&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1661.96,-1008C1978.98,-1008 2656.68,-1008 2656.68,-1008 2656.68,-1008 2656.68,-838.43 2656.68,-838.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2659.31,-838.43 2656.68,-830.93 2654.06,-838.43 2659.31,-838.43"/>
</g>
<!-- m_tools&#45;&gt;m_process -->
<g id="edge18" class="edge">
<title>m_tools&#45;&gt;m_process</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1502.02,-648.4C1502.02,-648.4 1502.02,-538.45 1502.02,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1504.65,-538.45 1502.02,-530.95 1499.4,-538.45 1504.65,-538.45"/>
</g>
<!-- m_process&#45;&gt;m_paths -->
<g id="edge19" class="edge">
<title>m_process&#45;&gt;m_paths</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1502.02,-348.4C1502.02,-348.4 1502.02,-238.45 1502.02,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1504.65,-238.45 1502.02,-230.95 1499.4,-238.45 1504.65,-238.45"/>
</g>
<!-- m_process&#45;&gt;c_log -->
<g id="edge20" class="edge">
<title>m_process&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1661.81,-438C1990.12,-438 2710.02,-438 2710.02,-438 2710.02,-438 2710.02,-238.13 2710.02,-238.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2712.65,-238.13 2710.02,-230.63 2707.4,-238.13 2712.65,-238.13"/>
</g>
<!-- c_editor&#45;&gt;m_tools -->
<g id="edge1" class="edge">
<title>c_editor&#45;&gt;m_tools</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M190.02,-948.4C190.02,-860 190.02,-738 190.02,-738 190.02,-738 1331.6,-738 1331.6,-738"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1331.6,-740.63 1339.1,-738 1331.6,-735.38 1331.6,-740.63"/>
</g>
<!-- c_engine&#45;&gt;m_options -->
<g id="edge2" class="edge">
<title>c_engine&#45;&gt;m_options</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2763.36,-827.83C2763.36,-995.21 2763.36,-1338 2763.36,-1338 2763.36,-1338 2480.38,-1338 2480.38,-1338"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2480.38,-1335.38 2472.88,-1338 2480.38,-1340.63 2480.38,-1335.38"/>
</g>
<!-- c_storyboard&#45;&gt;m_options -->
<g id="edge3" class="edge">
<title>c_storyboard&#45;&gt;m_options</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2310.02,-1548.4C2310.02,-1548.4 2310.02,-1438.45 2310.02,-1438.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2312.65,-1438.45 2310.02,-1430.95 2307.4,-1438.45 2312.65,-1438.45"/>
</g>
<!-- c_update&#45;&gt;m_options -->
<g id="edge4" class="edge">
<title>c_update&#45;&gt;m_options</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2039.93,-1638C2071.46,-1638 2095.02,-1638 2095.02,-1638 2095.02,-1638 2095.02,-1338 2095.02,-1338 2095.02,-1338 2139.58,-1338 2139.58,-1338"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2139.58,-1340.63 2147.08,-1338 2139.58,-1335.38 2139.58,-1340.63"/>
</g>
<!-- c_update&#45;&gt;m_scratch -->
<g id="edge5" class="edge">
<title>c_update&#45;&gt;m_scratch</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1720.21,-1608C1617.26,-1608 1502.54,-1608 1502.54,-1608 1502.54,-1608 1502.54,-1438.43 1502.54,-1438.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1505.17,-1438.43 1502.54,-1430.93 1499.92,-1438.43 1505.17,-1438.43"/>
</g>
<!-- c_update&#45;&gt;m_version -->
<g id="edge6" class="edge">
<title>c_update&#45;&gt;m_version</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1880.02,-1548.4C1880.02,-1548.4 1880.02,-1438.45 1880.02,-1438.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1882.65,-1438.45 1880.02,-1430.95 1877.4,-1438.45 1882.65,-1438.45"/>
</g>
<!-- c_path&#45;&gt;m_scratch -->
<g id="edge9" class="edge">
<title>c_path&#45;&gt;m_scratch</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M175.02,-1248.15C175.02,-1216.2 175.02,-1188 175.02,-1188 175.02,-1188 1316,-1188 1316,-1188 1316,-1188 1316,-1237.62 1316,-1237.62"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1313.38,-1237.62 1316,-1245.12 1318.63,-1237.62 1313.38,-1237.62"/>
</g>
<!-- c_path&#45;&gt;m_pack -->
<g id="edge10" class="edge">
<title>c_path&#45;&gt;m_pack</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M319.76,-1308C359.01,-1308 390.02,-1308 390.02,-1308 390.02,-1308 390.02,-1008 390.02,-1008 390.02,-1008 1331.58,-1008 1331.58,-1008"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1331.58,-1010.63 1339.08,-1008 1331.58,-1005.38 1331.58,-1010.63"/>
</g>
<!-- c_window&#45;&gt;m_scratch -->
<g id="edge11" class="edge">
<title>c_window&#45;&gt;m_scratch</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1342.52,-1548.4C1342.52,-1548.4 1342.52,-1438.45 1342.52,-1438.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1345.15,-1438.45 1342.52,-1430.95 1339.9,-1438.45 1345.15,-1438.45"/>
</g>
<!-- c_window&#45;&gt;m_version -->
<g id="edge12" class="edge">
<title>c_window&#45;&gt;m_version</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1394.83,-1668C1526.27,-1668 1691.02,-1668 1691.02,-1668 1691.02,-1668 1691.02,-1338 1691.02,-1338 1691.02,-1338 1709.61,-1338 1709.61,-1338"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1709.61,-1340.63 1717.11,-1338 1709.61,-1335.38 1709.61,-1340.63"/>
</g>
<!-- c_window&#45;&gt;m_library -->
<g id="edge13" class="edge">
<title>c_window&#45;&gt;m_library</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1127.52,-1548.4C1127.52,-1548.4 1127.52,-1438.45 1127.52,-1438.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1130.15,-1438.45 1127.52,-1430.95 1124.9,-1438.45 1130.15,-1438.45"/>
</g>
</g>
</svg>
`;case`c_storyboard`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="1228pt" height="1179pt"
 viewBox="0.00 0.00 1228.00 1179.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 1164.25)">
<g id="clust1" class="cluster">
<title>cluster_c_storyboard</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-260 8,-1141.2 838,-1141.2 838,-260 8,-260"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-1128.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">STORYBOARD</text>
</g>
<!-- m_main -->
<g id="node1" class="node">
<title>m_main</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-1080 47.98,-1080 47.98,-900 368.02,-900 368.02,-1080"/>
<text xml:space="preserve" text-anchor="start" x="186.33" y="-1002.8" font-family="Arial" font-size="20.00" fill="#f8fafc">main</text>
<text xml:space="preserve" text-anchor="start" x="181.63" y="-981.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">main.cpp</text>
<text xml:space="preserve" text-anchor="start" x="100.67" y="-960.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Runs a recipe&#39;s loupe.* steps on</text>
</g>
<!-- m_runner -->
<g id="node2" class="node">
<title>m_runner</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-780 47.98,-780 47.98,-600 368.02,-600 368.02,-780"/>
<text xml:space="preserve" text-anchor="start" x="179.09" y="-702.8" font-family="Arial" font-size="20.00" fill="#f8fafc">runner</text>
<text xml:space="preserve" text-anchor="start" x="141.87" y="-681.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">runner.cpp, runner.hpp</text>
<text xml:space="preserve" text-anchor="start" x="104.87" y="-660.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">ImVec2&#39;s arithmetic, for the cro</text>
</g>
<!-- m_recipe -->
<g id="node3" class="node">
<title>m_recipe</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-480 47.98,-480 47.98,-300 368.02,-300 368.02,-480"/>
<text xml:space="preserve" text-anchor="start" x="180.76" y="-402.8" font-family="Arial" font-size="20.00" fill="#f8fafc">recipe</text>
<text xml:space="preserve" text-anchor="start" x="144.04" y="-381.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">recipe.cpp, recipe.hpp</text>
<text xml:space="preserve" text-anchor="start" x="103.79" y="-360.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A recipe: the YAML list of steps</text>
</g>
<!-- c_config -->
<g id="node4" class="node">
<title>c_config</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="798.02,-1080 477.98,-1080 477.98,-900 798.02,-900 798.02,-1080"/>
<text xml:space="preserve" text-anchor="start" x="561.29" y="-1002.8" font-family="Arial" font-size="20.00" fill="#eff6ff">storyboard/config</text>
<text xml:space="preserve" text-anchor="start" x="598.26" y="-981.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">1 file · 6 lines</text>
<text xml:space="preserve" text-anchor="start" x="535.27" y="-960.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Dear ImGui&#39;s configuration in a</text>
</g>
<!-- c_window -->
<g id="node5" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1198.02,-480 877.98,-480 877.98,-300 1198.02,-300 1198.02,-480"/>
<text xml:space="preserve" text-anchor="start" x="1004.65" y="-402.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="980.55" y="-381.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">20 files · 3514 lines</text>
<text xml:space="preserve" text-anchor="start" x="928.6" y="-360.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_src -->
<g id="node6" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="368.02,-180 47.98,-180 47.98,0 368.02,0 368.02,-180"/>
<text xml:space="preserve" text-anchor="start" x="194.67" y="-91.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="150.55" y="-70.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- m_main&#45;&gt;m_runner -->
<g id="edge1" class="edge">
<title>m_main&#45;&gt;m_runner</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M208,-900.4C208,-900.4 208,-790.45 208,-790.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="210.63,-790.45 208,-782.95 205.38,-790.45 210.63,-790.45"/>
</g>
<!-- m_runner&#45;&gt;m_recipe -->
<g id="edge2" class="edge">
<title>m_runner&#45;&gt;m_recipe</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M208,-600.4C208,-600.4 208,-490.45 208,-490.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="210.63,-490.45 208,-482.95 205.38,-490.45 210.63,-490.45"/>
</g>
<!-- m_runner&#45;&gt;c_window -->
<g id="edge3" class="edge">
<title>m_runner&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M367.54,-690C608.64,-690 1038,-690 1038,-690 1038,-690 1038,-490.13 1038,-490.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1040.63,-490.13 1038,-482.63 1035.38,-490.13 1040.63,-490.13"/>
</g>
<!-- m_recipe&#45;&gt;c_src -->
<g id="edge4" class="edge">
<title>m_recipe&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M208,-300.4C208,-300.4 208,-190.45 208,-190.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="210.63,-190.45 208,-182.95 205.38,-190.45 210.63,-190.45"/>
</g>
</g>
</svg>
`;case`c_storyboard_config`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="430pt" height="311pt"
 viewBox="0.00 0.00 430.00 311.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 296.25)">
<g id="clust1" class="cluster">
<title>cluster_c_config</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-8 8,-273.2 392,-273.2 392,-8 8,-8"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-260.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">STORYBOARD/CONFIG</text>
</g>
<!-- m_imgui_config -->
<g id="node1" class="node">
<title>m_imgui_config</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="360.02,-220 39.98,-220 39.98,-40 360.02,-40 360.02,-220"/>
<text xml:space="preserve" text-anchor="start" x="143.86" y="-142.8" font-family="Arial" font-size="20.00" fill="#f8fafc">imgui_config</text>
<text xml:space="preserve" text-anchor="start" x="158.09" y="-121.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">imgui_config.h</text>
<text xml:space="preserve" text-anchor="start" x="97.27" y="-100.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Dear ImGui&#39;s configuration in a</text>
</g>
</g>
</svg>
`;case`c_tests`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="30pt" height="30pt"
 viewBox="0.00 0.00 30.00 30.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 15.05)">
</g>
</svg>
`;case`c_update`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="1043pt" height="810pt"
 viewBox="0.00 0.00 1043.00 810.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 795.05)">
<g id="clust1" class="cluster">
<title>cluster_c_update</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="175.02,-260 175.02,-541.2 1005.02,-541.2 1005.02,-260 175.02,-260"/>
<text xml:space="preserve" text-anchor="start" x="183.02" y="-528.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">UPDATE</text>
</g>
<!-- m_update -->
<g id="node1" class="node">
<title>m_update</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="535.04,-480 215,-480 215,-300 535.04,-300 535.04,-480"/>
<text xml:space="preserve" text-anchor="start" x="344.43" y="-402.8" font-family="Arial" font-size="20.00" fill="#f8fafc">update</text>
<text xml:space="preserve" text-anchor="start" x="306.71" y="-381.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">update.cpp, update.hpp</text>
<text xml:space="preserve" text-anchor="start" x="272.05" y="-360.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Loupe keeps itself current from</text>
</g>
<!-- m_sha256 -->
<g id="node2" class="node">
<title>m_sha256</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="965.04,-480 645,-480 645,-300 965.04,-300 965.04,-480"/>
<text xml:space="preserve" text-anchor="start" x="772.21" y="-402.8" font-family="Arial" font-size="20.00" fill="#f8fafc">sha256</text>
<text xml:space="preserve" text-anchor="start" x="733.82" y="-381.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">sha256.cpp, sha256.hpp</text>
<text xml:space="preserve" text-anchor="start" x="695.37" y="-360.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">For checking a download against</text>
</g>
<!-- c_window -->
<g id="node3" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="535.04,-780 215,-780 215,-600 535.04,-600 535.04,-780"/>
<text xml:space="preserve" text-anchor="start" x="341.67" y="-702.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="317.57" y="-681.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">20 files · 3514 lines</text>
<text xml:space="preserve" text-anchor="start" x="265.62" y="-660.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_log -->
<g id="node4" class="node">
<title>c_log</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-180 0,-180 0,0 320.04,0 320.04,-180"/>
<text xml:space="preserve" text-anchor="start" x="146.68" y="-102.8" font-family="Arial" font-size="20.00" fill="#eff6ff">log</text>
<text xml:space="preserve" text-anchor="start" x="109.8" y="-81.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 611 lines</text>
<text xml:space="preserve" text-anchor="start" x="55.61" y="-60.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe&#39;s log: one sink every line</text>
</g>
<!-- c_src -->
<g id="node5" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="750.04,-180 430,-180 430,0 750.04,0 750.04,-180"/>
<text xml:space="preserve" text-anchor="start" x="576.69" y="-91.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="532.57" y="-70.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- m_update&#45;&gt;m_sha256 -->
<g id="edge2" class="edge">
<title>m_update&#45;&gt;m_sha256</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M534.83,-390C534.83,-390 634.74,-390 634.74,-390"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="634.74,-392.63 642.24,-390 634.74,-387.38 634.74,-392.63"/>
</g>
<!-- m_update&#45;&gt;c_log -->
<g id="edge3" class="edge">
<title>m_update&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M267.52,-300.4C267.52,-300.4 267.52,-190.45 267.52,-190.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="270.15,-190.45 267.52,-182.95 264.9,-190.45 270.15,-190.45"/>
</g>
<!-- m_update&#45;&gt;c_src -->
<g id="edge4" class="edge">
<title>m_update&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M482.52,-300.4C482.52,-300.4 482.52,-190.45 482.52,-190.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="485.15,-190.45 482.52,-182.95 479.9,-190.45 485.15,-190.45"/>
</g>
<!-- c_window&#45;&gt;m_update -->
<g id="edge1" class="edge">
<title>c_window&#45;&gt;m_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M375.02,-600.4C375.02,-600.4 375.02,-490.45 375.02,-490.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="377.65,-490.45 375.02,-482.95 372.4,-490.45 377.65,-490.45"/>
</g>
</g>
</svg>
`;case`c_window`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="3552pt" height="1458pt"
 viewBox="0.00 0.00 3552.00 1458.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 1443.05)">
<g id="clust1" class="cluster">
<title>cluster_c_window</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="790.02,-8 790.02,-1189.2 2910.02,-1189.2 2910.02,-8 790.02,-8"/>
<text xml:space="preserve" text-anchor="start" x="798.02" y="-1176.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">WINDOW</text>
</g>
<!-- m_run_window -->
<g id="node1" class="node">
<title>m_run_window</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="2225.04,-1128 1905,-1128 1905,-948 2225.04,-948 2225.04,-1128"/>
<text xml:space="preserve" text-anchor="start" x="2011.66" y="-1050.8" font-family="Arial" font-size="20.00" fill="#f8fafc">run_window</text>
<text xml:space="preserve" text-anchor="start" x="1967.1" y="-1029.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">run_window.cpp, run_window.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1955.62" y="-1008.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The window: a scene&#39;s frame, a t</text>
</g>
<!-- m_console -->
<g id="node2" class="node">
<title>m_console</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="2010.04,-828 1690,-828 1690,-648 2010.04,-648 2010.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="1815.55" y="-750.8" font-family="Arial" font-size="20.00" fill="#f8fafc">console</text>
<text xml:space="preserve" text-anchor="start" x="1776.66" y="-729.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">console.cpp, console.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1745.61" y="-708.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The Console panel: the log&#39;s lin</text>
</g>
<!-- m_editor -->
<g id="node3" class="node">
<title>m_editor</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="2440.04,-828 2120,-828 2120,-648 2440.04,-648 2440.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="2255.01" y="-750.8" font-family="Arial" font-size="20.00" fill="#f8fafc">editor</text>
<text xml:space="preserve" text-anchor="start" x="2218.95" y="-729.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">editor.cpp, editor.hpp</text>
<text xml:space="preserve" text-anchor="start" x="2171.86" y="-708.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The scene editor&#39;s panels over a</text>
</g>
<!-- m_files -->
<g id="node4" class="node">
<title>m_files</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="2870.04,-828 2550,-828 2550,-648 2870.04,-648 2870.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="2692.24" y="-750.8" font-family="Arial" font-size="20.00" fill="#f8fafc">files</text>
<text xml:space="preserve" text-anchor="start" x="2658.35" y="-729.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">files.cpp, files.hpp</text>
<text xml:space="preserve" text-anchor="start" x="2603.12" y="-708.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The window&#39;s files through SDL:</text>
</g>
<!-- m_frame_texture -->
<g id="node5" class="node">
<title>m_frame_texture</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1580.04,-828 1260,-828 1260,-648 1580.04,-648 1580.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="1358.33" y="-750.8" font-family="Arial" font-size="20.00" fill="#f8fafc">frame_texture</text>
<text xml:space="preserve" text-anchor="start" x="1311.27" y="-729.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">frame_texture.cpp, frame_texture.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1310.61" y="-708.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The engine&#39;s frame as one textur</text>
</g>
<!-- m_observer -->
<g id="node6" class="node">
<title>m_observer</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1150.04,-828 830,-828 830,-648 1150.04,-648 1150.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="951.11" y="-750.8" font-family="Arial" font-size="20.00" fill="#f8fafc">observer</text>
<text xml:space="preserve" text-anchor="start" x="910.89" y="-729.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">observer.cpp, observer.hpp</text>
<text xml:space="preserve" text-anchor="start" x="879.37" y="-708.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The observer: the person&#39;s own c</text>
</g>
<!-- m_path_editor -->
<g id="node7" class="node">
<title>m_path_editor</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="2440.04,-528 2120,-528 2120,-348 2440.04,-348 2440.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="2229.98" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">path_editor</text>
<text xml:space="preserve" text-anchor="start" x="2186.42" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">path_editor.cpp, path_editor.hpp</text>
<text xml:space="preserve" text-anchor="start" x="2173.12" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The editor&#39;s camera paths: the s</text>
</g>
<!-- m_gpu -->
<g id="node8" class="node">
<title>m_gpu</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1580.04,-528 1260,-528 1260,-348 1580.04,-348 1580.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="1403.34" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">gpu</text>
<text xml:space="preserve" text-anchor="start" x="1369.78" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">gpu.cpp, gpu.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1306.46" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">After imgui&#39;s examples/example_s</text>
</g>
<!-- m_panels -->
<g id="node9" class="node">
<title>m_panels</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="2440.04,-228 2120,-228 2120,-48 2440.04,-48 2440.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="2250.55" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">panels</text>
<text xml:space="preserve" text-anchor="start" x="2213.16" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">panels.cpp, panels.hpp</text>
<text xml:space="preserve" text-anchor="start" x="2208.55" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The window&#39;s panels.</text>
</g>
<!-- m_fonts -->
<g id="node10" class="node">
<title>m_fonts</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1795.04,-228 1475,-228 1475,-48 1795.04,-48 1795.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="1613.34" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">fonts</text>
<text xml:space="preserve" text-anchor="start" x="1578.29" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">fonts.cpp, fonts.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1528.95" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The panels&#39; typefaces by role: T</text>
</g>
<!-- c_src -->
<g id="node11" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="3270.04,-528 2950,-528 2950,-348 3270.04,-348 3270.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="3096.69" y="-439.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="3052.57" y="-418.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1274 lines</text>
</g>
<!-- c_storyboard -->
<g id="node12" class="node">
<title>c_storyboard</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2225.04,-1428 1905,-1428 1905,-1248 2225.04,-1248 2225.04,-1428"/>
<text xml:space="preserve" text-anchor="start" x="2017.77" y="-1339.8" font-family="Arial" font-size="20.00" fill="#eff6ff">storyboard</text>
<text xml:space="preserve" text-anchor="start" x="2014.8" y="-1318.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 751 lines</text>
</g>
<!-- c_update -->
<g id="node13" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="3522.04,-828 3202,-828 3202,-648 3522.04,-648 3522.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="3331.43" y="-750.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="3311.8" y="-729.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 847 lines</text>
<text xml:space="preserve" text-anchor="start" x="3259.05" y="-708.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_log -->
<g id="node14" class="node">
<title>c_log</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-228 0,-228 0,-48 320.04,-48 320.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="146.68" y="-150.8" font-family="Arial" font-size="20.00" fill="#eff6ff">log</text>
<text xml:space="preserve" text-anchor="start" x="109.8" y="-129.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 611 lines</text>
<text xml:space="preserve" text-anchor="start" x="55.61" y="-108.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe&#39;s log: one sink every line</text>
</g>
<!-- c_engine -->
<g id="node15" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="750.04,-528 430,-528 430,-348 750.04,-348 750.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="559.99" y="-439.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="539.8" y="-418.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 994 lines</text>
</g>
<!-- c_editor -->
<g id="node16" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="750.04,-228 430,-228 430,-48 750.04,-48 750.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="565.01" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="536.18" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">8 files · 1052 lines</text>
</g>
<!-- m_run_window&#45;&gt;m_console -->
<g id="edge3" class="edge">
<title>m_run_window&#45;&gt;m_console</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1957.52,-948.4C1957.52,-948.4 1957.52,-838.45 1957.52,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1960.15,-838.45 1957.52,-830.95 1954.9,-838.45 1960.15,-838.45"/>
</g>
<!-- m_run_window&#45;&gt;m_editor -->
<g id="edge4" class="edge">
<title>m_run_window&#45;&gt;m_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2172.52,-948.4C2172.52,-948.4 2172.52,-838.45 2172.52,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2175.15,-838.45 2172.52,-830.95 2169.9,-838.45 2175.15,-838.45"/>
</g>
<!-- m_run_window&#45;&gt;m_files -->
<g id="edge5" class="edge">
<title>m_run_window&#45;&gt;m_files</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2224.87,-993C2415.89,-993 2710.02,-993 2710.02,-993 2710.02,-993 2710.02,-838.43 2710.02,-838.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2712.65,-838.43 2710.02,-830.93 2707.4,-838.43 2712.65,-838.43"/>
</g>
<!-- m_run_window&#45;&gt;m_frame_texture -->
<g id="edge6" class="edge">
<title>m_run_window&#45;&gt;m_frame_texture</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1905.17,-1008C1714.15,-1008 1420.02,-1008 1420.02,-1008 1420.02,-1008 1420.02,-838.43 1420.02,-838.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1422.65,-838.43 1420.02,-830.93 1417.4,-838.43 1422.65,-838.43"/>
</g>
<!-- m_run_window&#45;&gt;m_observer -->
<g id="edge7" class="edge">
<title>m_run_window&#45;&gt;m_observer</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1905.24,-1068C1605.78,-1068 990.02,-1068 990.02,-1068 990.02,-1068 990.02,-838.27 990.02,-838.27"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="992.65,-838.27 990.02,-830.77 987.4,-838.27 992.65,-838.27"/>
</g>
<!-- m_run_window&#45;&gt;c_update -->
<g id="edge8" class="edge">
<title>m_run_window&#45;&gt;c_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2225.03,-1083C2571.88,-1083 3362.02,-1083 3362.02,-1083 3362.02,-1083 3362.02,-838.27 3362.02,-838.27"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="3364.65,-838.27 3362.02,-830.77 3359.4,-838.27 3364.65,-838.27"/>
</g>
<!-- m_console&#45;&gt;m_fonts -->
<g id="edge9" class="edge">
<title>m_console&#45;&gt;m_fonts</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1760.03,-648.17C1760.03,-648.17 1760.03,-238.48 1760.03,-238.48"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1762.65,-238.48 1760.03,-230.98 1757.4,-238.48 1762.65,-238.48"/>
</g>
<!-- m_console&#45;&gt;c_log -->
<g id="edge10" class="edge">
<title>m_console&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1725.01,-648.15C1725.01,-616.2 1725.01,-588 1725.01,-588 1725.01,-588 106.68,-588 106.68,-588 106.68,-588 106.68,-238.51 106.68,-238.51"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="109.31,-238.51 106.68,-231.01 104.06,-238.51 109.31,-238.51"/>
</g>
<!-- m_editor&#45;&gt;m_path_editor -->
<g id="edge11" class="edge">
<title>m_editor&#45;&gt;m_path_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2280.02,-648.4C2280.02,-648.4 2280.02,-538.45 2280.02,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2282.65,-538.45 2280.02,-530.95 2277.4,-538.45 2282.65,-538.45"/>
</g>
<!-- m_files&#45;&gt;c_src -->
<g id="edge12" class="edge">
<title>m_files&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2869.64,-738C2951.42,-738 3034,-738 3034,-738 3034,-738 3034,-538.13 3034,-538.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="3036.63,-538.13 3034,-530.63 3031.38,-538.13 3036.63,-538.13"/>
</g>
<!-- m_frame_texture&#45;&gt;m_gpu -->
<g id="edge13" class="edge">
<title>m_frame_texture&#45;&gt;m_gpu</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1420.02,-648.4C1420.02,-648.4 1420.02,-538.45 1420.02,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1422.65,-538.45 1420.02,-530.95 1417.4,-538.45 1422.65,-538.45"/>
</g>
<!-- m_frame_texture&#45;&gt;c_engine -->
<g id="edge14" class="edge">
<title>m_frame_texture&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1260.11,-738C1228.58,-738 1205.02,-738 1205.02,-738 1205.02,-738 1205.02,-408 1205.02,-408 1205.02,-408 760.53,-408 760.53,-408"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="760.53,-405.38 753.03,-408 760.53,-410.63 760.53,-405.38"/>
</g>
<!-- m_observer&#45;&gt;c_engine -->
<g id="edge15" class="edge">
<title>m_observer&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M990.02,-648.09C990.02,-569.55 990.02,-468 990.02,-468 990.02,-468 760.36,-468 760.36,-468"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="760.36,-465.38 752.86,-468 760.36,-470.63 760.36,-465.38"/>
</g>
<!-- m_path_editor&#45;&gt;m_panels -->
<g id="edge16" class="edge">
<title>m_path_editor&#45;&gt;m_panels</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2333.36,-348.4C2333.36,-348.4 2333.36,-238.45 2333.36,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2335.99,-238.45 2333.36,-230.95 2330.74,-238.45 2335.99,-238.45"/>
</g>
<!-- m_path_editor&#45;&gt;c_editor -->
<g id="edge17" class="edge">
<title>m_path_editor&#45;&gt;c_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2226.68,-348.02C2226.68,-307.24 2226.68,-268 2226.68,-268 2226.68,-268 590.02,-268 590.02,-268 590.02,-268 590.02,-238.46 590.02,-238.46"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="592.65,-238.46 590.02,-230.96 587.4,-238.46 592.65,-238.46"/>
</g>
<!-- m_gpu&#45;&gt;m_fonts -->
<g id="edge18" class="edge">
<title>m_gpu&#45;&gt;m_fonts</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1527.52,-348.4C1527.52,-348.4 1527.52,-238.45 1527.52,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1530.15,-238.45 1527.52,-230.95 1524.9,-238.45 1530.15,-238.45"/>
</g>
<!-- m_gpu&#45;&gt;c_log -->
<g id="edge19" class="edge">
<title>m_gpu&#45;&gt;c_log</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1367.5,-348.07C1367.5,-325.71 1367.5,-308 1367.5,-308 1367.5,-308 213.36,-308 213.36,-308 213.36,-308 213.36,-238.51 213.36,-238.51"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="215.99,-238.5 213.36,-231.01 210.74,-238.51 215.99,-238.5"/>
</g>
<!-- m_panels&#45;&gt;c_src -->
<g id="edge20" class="edge">
<title>m_panels&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2439.56,-138C2680.66,-138 3110.02,-138 3110.02,-138 3110.02,-138 3110.02,-337.87 3110.02,-337.87"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="3107.4,-337.87 3110.02,-345.37 3112.65,-337.87 3107.4,-337.87"/>
</g>
<!-- c_src&#45;&gt;m_run_window -->
<g id="edge1" class="edge">
<title>c_src&#45;&gt;m_run_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M3118,-527.83C3118,-695.21 3118,-1038 3118,-1038 3118,-1038 2235.47,-1038 2235.47,-1038"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2235.47,-1035.38 2227.97,-1038 2235.47,-1040.63 2235.47,-1035.38"/>
</g>
<!-- c_storyboard&#45;&gt;m_run_window -->
<g id="edge2" class="edge">
<title>c_storyboard&#45;&gt;m_run_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2065.02,-1248.4C2065.02,-1248.4 2065.02,-1138.45 2065.02,-1138.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2067.65,-1138.45 2065.02,-1130.95 2062.4,-1138.45 2067.65,-1138.45"/>
</g>
</g>
</svg>
`;default:throw Error(`Unknown viewId: `+e)}};export{e as dotSource,t as svgSource};