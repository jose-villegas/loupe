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
        c_window [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">14 files · 1962 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window",
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
        c_editor [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 547 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_editor",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_engine [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 766 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_update [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 853 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_update",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_src [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1249 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
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
    c_window -> c_update [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=wuxb7b,
        style=solid];
    c_path -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=nher8l,
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
    c_src -> c_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="18fjin8",
        style=solid];
    c_src -> c_path [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1kireed",
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
        c_window [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">14 files · 1962 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_editor [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 547 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_editor",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_update [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 853 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_update",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_engine [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 766 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        c_src [fixedsize=true,
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1249 lines</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_src",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_tests -> c_path [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1y7tcgy",
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
    c_engine -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=zr62cm,
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
    }
    c_window [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">14 files · 1962 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_window",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_window -> m_placement [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="183w5iq",
        style=solid];
    c_window -> m_scene_tool [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="300m9u",
        style=solid];
    c_engine [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 766 lines</FONT></TD></TR></TABLE>>,
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
    m_placement -> m_doc [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1img2z",
        style=solid,
        weight=2];
    m_scene_tool -> m_doc [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=quj5y3,
        style=solid,
        weight=2];
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1249 lines</FONT></TD></TR></TABLE>>,
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
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">frames</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">frames.cpp, frames.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Gfx's frame from a provider, dra</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine.m_frames",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_autana [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
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
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">pose</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">pose.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">Autana's packages (launcher/pack</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_engine.m_pose",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_editor [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 547 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_editor",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_editor -> m_frames [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="197q00n",
        minlen=1,
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
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1249 lines</FONT></TD></TR></TABLE>>,
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
        minlen=1,
        style=solid];
    c_window [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">14 files · 1962 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_window",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_window -> m_frames [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1pi6fcq",
        minlen=1,
        style=solid];
    m_frames -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1yauvf8",
        style=solid];
    m_frames -> m_pose [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=l8yigd,
        minlen=0,
        style=solid,
        weight=2];
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
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1249 lines</FONT></TD></TR></TABLE>>,
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
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 766 lines</FONT></TD></TR></TABLE>>,
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
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 547 lines</FONT></TD></TR></TABLE>>,
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
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 766 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_engine",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_engine -> m_options [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=vpu8y2,
        style=solid];
    c_update [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 853 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
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
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">14 files · 1962 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
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
        style=solid];
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
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">14 files · 1962 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
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
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1249 lines</FONT></TD></TR></TABLE>>,
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
            group="loupe.loupe.c_window",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">run_window</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">run_window.cpp, run_window.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The window: a scene's frame, a t</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_run_window",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_editor [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_window",
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
            group="loupe.loupe.c_window",
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
            group="loupe.loupe.c_window",
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
            group="loupe.loupe.c_window",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">observer</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">observer.cpp, observer.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The observer: the person's own c</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_observer",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_panels [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_window",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">panels</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">panels.cpp, panels.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">The window's panels.</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_panels",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
        m_gpu [color="#475569",
            fillcolor="#64748b",
            fixedsize=true,
            fontcolor="#f8fafc",
            group="loupe.loupe.c_window",
            height=2.5,
            label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">gpu</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#cbd5e1">gpu.cpp, gpu.hpp</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#cbd5e1">After imgui's examples/example_s</FONT></TD></TR></TABLE>>,
            likec4_id="loupe.loupe.c_window.m_gpu",
            likec4_level=1,
            margin="0.223,0.223",
            width=4.445];
    }
    c_src [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">src</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">17 files · 1249 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_src",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    c_src -> m_run_window [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=zyy7s2,
        style=solid];
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
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">update</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 853 lines</FONT></TD></TR><TR><TD><FONT POINT-SIZE="15" COLOR="#bfdbfe">Loupe keeps itself current from</FONT></TD></TR></TABLE>>,
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
    m_editor -> m_panels [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1ml5hgu",
        style=solid,
        weight=2];
    c_editor [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">editor</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">6 files · 547 lines</FONT></TD></TR></TABLE>>,
        likec4_id="loupe.loupe.c_editor",
        likec4_level=0,
        margin="0.223,0.223",
        width=4.445];
    m_editor -> c_editor [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id=scv7yy,
        minlen=1,
        style=solid];
    m_files -> c_src [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="19rm5m5",
        style=solid];
    m_frame_texture -> m_gpu [arrowhead=normal,
        color="#64748b",
        fontcolor="#cbd5e1",
        likec4_id="1mtv6lx",
        minlen=1,
        style=solid,
        weight=2];
    c_engine [fixedsize=true,
        height=2.5,
        label=<<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="4"><TR><TD><FONT POINT-SIZE="20">engine</FONT></TD></TR><TR><TD><FONT POINT-SIZE="13" COLOR="#bfdbfe">4 files · 766 lines</FONT></TD></TR></TABLE>>,
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
<svg width="1418pt" height="1227pt"
 viewBox="0.00 0.00 1418.00 1227.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 1212.25)">
<g id="clust1" class="cluster">
<title>cluster_loupe</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-8 8,-1189.2 1380,-1189.2 1380,-8 8,-8"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-1176.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">LOUPE</text>
</g>
<!-- c_window -->
<g id="node1" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1013.02,-1128 692.98,-1128 692.98,-948 1013.02,-948 1013.02,-1128"/>
<text xml:space="preserve" text-anchor="start" x="819.65" y="-1050.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="795.55" y="-1029.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">14 files · 1962 lines</text>
<text xml:space="preserve" text-anchor="start" x="743.6" y="-1008.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_path -->
<g id="node2" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1013.02,-228 692.98,-228 692.98,-48 1013.02,-48 1013.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="833.54" y="-150.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="802.78" y="-129.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="741.52" y="-108.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_editor -->
<g id="node3" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="368.02,-828 47.98,-828 47.98,-648 368.02,-648 368.02,-828"/>
<text xml:space="preserve" text-anchor="start" x="182.99" y="-739.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="157.78" y="-718.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 547 lines</text>
</g>
<!-- c_engine -->
<g id="node4" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="798.02,-828 477.98,-828 477.98,-648 798.02,-648 798.02,-828"/>
<text xml:space="preserve" text-anchor="start" x="607.97" y="-739.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="587.78" y="-718.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 766 lines</text>
</g>
<!-- c_update -->
<g id="node5" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1228.02,-828 907.98,-828 907.98,-648 1228.02,-648 1228.02,-828"/>
<text xml:space="preserve" text-anchor="start" x="1037.41" y="-750.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="1017.78" y="-729.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 853 lines</text>
<text xml:space="preserve" text-anchor="start" x="965.03" y="-708.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_src -->
<g id="node6" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1013.02,-528 692.98,-528 692.98,-348 1013.02,-348 1013.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="839.67" y="-439.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="795.55" y="-418.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1249 lines</text>
</g>
<!-- c_window&#45;&gt;c_editor -->
<g id="edge1" class="edge">
<title>c_window&#45;&gt;c_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M693.15,-1038C502.13,-1038 208,-1038 208,-1038 208,-1038 208,-838.13 208,-838.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="210.63,-838.13 208,-830.63 205.38,-838.13 210.63,-838.13"/>
</g>
<!-- c_window&#45;&gt;c_engine -->
<g id="edge2" class="edge">
<title>c_window&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M745.5,-948.4C745.5,-948.4 745.5,-838.45 745.5,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="748.13,-838.45 745.5,-830.95 742.88,-838.45 748.13,-838.45"/>
</g>
<!-- c_window&#45;&gt;c_update -->
<g id="edge3" class="edge">
<title>c_window&#45;&gt;c_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M960.5,-948.4C960.5,-948.4 960.5,-838.45 960.5,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="963.13,-838.45 960.5,-830.95 957.88,-838.45 963.13,-838.45"/>
</g>
<!-- c_path&#45;&gt;c_src -->
<g id="edge4" class="edge">
<title>c_path&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M906.34,-227.6C906.34,-227.6 906.34,-337.55 906.34,-337.55"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="903.72,-337.55 906.34,-345.05 908.97,-337.55 903.72,-337.55"/>
</g>
<!-- c_editor&#45;&gt;c_src -->
<g id="edge5" class="edge">
<title>c_editor&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M208,-648.4C208,-560 208,-438 208,-438 208,-438 682.62,-438 682.62,-438"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="682.62,-440.63 690.12,-438 682.62,-435.38 682.62,-440.63"/>
</g>
<!-- c_engine&#45;&gt;c_src -->
<g id="edge6" class="edge">
<title>c_engine&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M745.5,-648.4C745.5,-648.4 745.5,-538.45 745.5,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="748.13,-538.45 745.5,-530.95 742.88,-538.45 748.13,-538.45"/>
</g>
<!-- c_update&#45;&gt;c_src -->
<g id="edge7" class="edge">
<title>c_update&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M960.5,-648.4C960.5,-648.4 960.5,-538.45 960.5,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="963.13,-538.45 960.5,-530.95 957.88,-538.45 963.13,-538.45"/>
</g>
<!-- c_src&#45;&gt;c_window -->
<g id="edge8" class="edge">
<title>c_src&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M853,-527.83C853,-527.83 853,-937.52 853,-937.52"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="850.38,-937.52 853,-945.02 855.63,-937.52 850.38,-937.52"/>
</g>
<!-- c_src&#45;&gt;c_path -->
<g id="edge9" class="edge">
<title>c_src&#45;&gt;c_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M799.66,-348.4C799.66,-348.4 799.66,-238.45 799.66,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="802.29,-238.45 799.66,-230.95 797.04,-238.45 802.29,-238.45"/>
</g>
</g>
</svg>
`;case`loupe_all`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="1006pt" height="1827pt"
 viewBox="0.00 0.00 1006.00 1827.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 1812.25)">
<g id="clust1" class="cluster">
<title>cluster_loupe</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-8 8,-1789.2 968,-1789.2 968,-8 8,-8"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-1776.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">LOUPE</text>
</g>
<!-- c_tests -->
<g id="node1" class="node">
<title>c_tests</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="489.02,-1728 168.98,-1728 168.98,-1548 489.02,-1548 489.02,-1728"/>
<text xml:space="preserve" text-anchor="start" x="307.88" y="-1639.8" font-family="Arial" font-size="20.00" fill="#eff6ff">tests</text>
<text xml:space="preserve" text-anchor="start" x="286.01" y="-1618.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">0 files · 0 lines</text>
</g>
<!-- c_path -->
<g id="node2" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="816.02,-1428 495.98,-1428 495.98,-1248 816.02,-1248 816.02,-1428"/>
<text xml:space="preserve" text-anchor="start" x="636.54" y="-1350.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="605.78" y="-1329.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="544.52" y="-1308.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_window -->
<g id="node3" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="592.02,-528 271.98,-528 271.98,-348 592.02,-348 592.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="398.65" y="-450.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="374.55" y="-429.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">14 files · 1962 lines</text>
<text xml:space="preserve" text-anchor="start" x="322.6" y="-408.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_editor -->
<g id="node4" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="433.02,-228 112.98,-228 112.98,-48 433.02,-48 433.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="247.99" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="222.78" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 547 lines</text>
</g>
<!-- c_update -->
<g id="node5" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="928.02,-228 607.98,-228 607.98,-48 928.02,-48 928.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="737.41" y="-150.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="717.78" y="-129.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 853 lines</text>
<text xml:space="preserve" text-anchor="start" x="665.03" y="-108.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_engine -->
<g id="node6" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="592.02,-1128 271.98,-1128 271.98,-948 592.02,-948 592.02,-1128"/>
<text xml:space="preserve" text-anchor="start" x="401.97" y="-1039.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="381.78" y="-1018.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 766 lines</text>
</g>
<!-- c_src -->
<g id="node7" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="816.02,-828 495.98,-828 495.98,-648 816.02,-648 816.02,-828"/>
<text xml:space="preserve" text-anchor="start" x="642.67" y="-739.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="598.55" y="-718.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1249 lines</text>
</g>
<!-- c_tests&#45;&gt;c_path -->
<g id="edge1" class="edge">
<title>c_tests&#45;&gt;c_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M416.67,-1548.4C416.67,-1460 416.67,-1338 416.67,-1338 416.67,-1338 485.51,-1338 485.51,-1338"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="485.51,-1340.63 493.01,-1338 485.51,-1335.38 485.51,-1340.63"/>
</g>
<!-- c_tests&#45;&gt;c_window -->
<g id="edge2" class="edge">
<title>c_tests&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M210.18,-1548.3C210.18,-1269.71 210.18,-438 210.18,-438 210.18,-438 261.74,-438 261.74,-438"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="261.74,-440.63 269.24,-438 261.74,-435.38 261.74,-440.63"/>
</g>
<!-- c_tests&#45;&gt;c_editor -->
<g id="edge3" class="edge">
<title>c_tests&#45;&gt;c_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M189.58,-1548.1C189.58,-1548.1 189.58,-238.25 189.58,-238.25"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="192.21,-238.25 189.58,-230.75 186.96,-238.25 192.21,-238.25"/>
</g>
<!-- c_tests&#45;&gt;c_update -->
<g id="edge4" class="edge">
<title>c_tests&#45;&gt;c_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M488.99,-1638C649.16,-1638 872.02,-1638 872.02,-1638 872.02,-1638 872.02,-238.42 872.02,-238.42"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="874.65,-238.42 872.02,-230.92 869.4,-238.42 874.65,-238.42"/>
</g>
<!-- c_tests&#45;&gt;c_engine -->
<g id="edge5" class="edge">
<title>c_tests&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M344.33,-1548.17C344.33,-1548.17 344.33,-1138.48 344.33,-1138.48"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="346.95,-1138.48 344.33,-1130.98 341.7,-1138.48 346.95,-1138.48"/>
</g>
<!-- c_tests&#45;&gt;c_src -->
<g id="edge6" class="edge">
<title>c_tests&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M230.78,-1548.41C230.78,-1320.77 230.78,-738 230.78,-738 230.78,-738 485.62,-738 485.62,-738"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="485.62,-740.63 493.12,-738 485.62,-735.38 485.62,-740.63"/>
</g>
<!-- c_path&#45;&gt;c_engine -->
<g id="edge7" class="edge">
<title>c_path&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M544,-1248.4C544,-1248.4 544,-1138.45 544,-1138.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="546.63,-1138.45 544,-1130.95 541.38,-1138.45 546.63,-1138.45"/>
</g>
<!-- c_path&#45;&gt;c_src -->
<g id="edge8" class="edge">
<title>c_path&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M666.69,-1248.17C666.69,-1248.17 666.69,-838.48 666.69,-838.48"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="669.31,-838.48 666.69,-830.98 664.06,-838.48 669.31,-838.48"/>
</g>
<!-- c_window&#45;&gt;c_editor -->
<g id="edge9" class="edge">
<title>c_window&#45;&gt;c_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M352.5,-348.4C352.5,-348.4 352.5,-238.45 352.5,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="355.13,-238.45 352.5,-230.95 349.88,-238.45 355.13,-238.45"/>
</g>
<!-- c_window&#45;&gt;c_update -->
<g id="edge10" class="edge">
<title>c_window&#45;&gt;c_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M512.52,-348.26C512.52,-250.54 512.52,-108 512.52,-108 512.52,-108 597.49,-108 597.49,-108"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="597.49,-110.63 604.99,-108 597.49,-105.38 597.49,-110.63"/>
</g>
<!-- c_window&#45;&gt;c_engine -->
<g id="edge11" class="edge">
<title>c_window&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M383.98,-527.83C383.98,-527.83 383.98,-937.52 383.98,-937.52"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="381.36,-937.52 383.98,-945.02 386.61,-937.52 381.36,-937.52"/>
</g>
<!-- c_window&#45;&gt;c_src -->
<g id="edge12" class="edge">
<title>c_window&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M560.01,-527.6C560.01,-527.6 560.01,-637.55 560.01,-637.55"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="557.38,-637.55 560.01,-645.05 562.63,-637.55 557.38,-637.55"/>
</g>
<!-- c_editor&#45;&gt;c_engine -->
<g id="edge13" class="edge">
<title>c_editor&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M251.38,-227.59C251.38,-455.23 251.38,-1038 251.38,-1038 251.38,-1038 261.57,-1038 261.57,-1038"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="261.57,-1040.63 269.07,-1038 261.57,-1035.38 261.57,-1040.63"/>
</g>
<!-- c_editor&#45;&gt;c_src -->
<g id="edge14" class="edge">
<title>c_editor&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M432.67,-168C515.68,-168 600,-168 600,-168 600,-168 600,-637.78 600,-637.78"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="597.38,-637.78 600,-645.28 602.63,-637.78 597.38,-637.78"/>
</g>
<!-- c_update&#45;&gt;c_src -->
<g id="edge15" class="edge">
<title>c_update&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M712,-227.83C712,-227.83 712,-637.52 712,-637.52"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="709.38,-637.52 712,-645.02 714.63,-637.52 709.38,-637.52"/>
</g>
<!-- c_engine&#45;&gt;c_src -->
<g id="edge16" class="edge">
<title>c_engine&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M527.99,-948.4C527.99,-948.4 527.99,-838.45 527.99,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="530.62,-838.45 527.99,-830.95 525.37,-838.45 530.62,-838.45"/>
</g>
<!-- c_src&#45;&gt;c_path -->
<g id="edge17" class="edge">
<title>c_src&#45;&gt;c_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M741.35,-827.83C741.35,-827.83 741.35,-1237.52 741.35,-1237.52"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="738.73,-1237.52 741.35,-1245.02 743.98,-1237.52 738.73,-1237.52"/>
</g>
<!-- c_src&#45;&gt;c_window -->
<g id="edge18" class="edge">
<title>c_src&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M527.99,-648.4C527.99,-648.4 527.99,-538.45 527.99,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="530.62,-538.45 527.99,-530.95 525.37,-538.45 530.62,-538.45"/>
</g>
<!-- c_src&#45;&gt;c_engine -->
<g id="edge19" class="edge">
<title>c_src&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M560.01,-827.6C560.01,-827.6 560.01,-937.55 560.01,-937.55"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="557.38,-937.55 560.01,-945.05 562.63,-937.55 557.38,-937.55"/>
</g>
</g>
</svg>
`;case`c_editor`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="1658pt" height="858pt"
 viewBox="0.00 0.00 1658.00 858.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 843.05)">
<g id="clust1" class="cluster">
<title>cluster_c_editor</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-8 8,-589.2 838,-589.2 838,-8 8,-8"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-576.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">EDITOR</text>
</g>
<!-- m_placement -->
<g id="node1" class="node">
<title>m_placement</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-528 47.98,-528 47.98,-348 368.02,-348 368.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="161.86" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">placement</text>
<text xml:space="preserve" text-anchor="start" x="119.47" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">placement.cpp, placement.hpp</text>
<text xml:space="preserve" text-anchor="start" x="96.27" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A Placement both ways: to the en</text>
</g>
<!-- m_scene_tool -->
<g id="node2" class="node">
<title>m_scene_tool</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="798.02,-528 477.98,-528 477.98,-348 798.02,-348 798.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="589.63" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">scene_tool</text>
<text xml:space="preserve" text-anchor="start" x="546.57" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">scene_tool.cpp, scene_tool.hpp</text>
<text xml:space="preserve" text-anchor="start" x="537.58" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The editor&#39;s calls into autana&#39;s</text>
</g>
<!-- m_doc -->
<g id="node3" class="node">
<title>m_doc</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="568.02,-228 247.98,-228 247.98,-48 568.02,-48 568.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="391.88" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">doc</text>
<text xml:space="preserve" text-anchor="start" x="358.49" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">doc.cpp, doc.hpp</text>
<text xml:space="preserve" text-anchor="start" x="298.77" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">What the editor holds and saves:</text>
</g>
<!-- c_window -->
<g id="node4" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="583.02,-828 262.98,-828 262.98,-648 583.02,-648 583.02,-828"/>
<text xml:space="preserve" text-anchor="start" x="389.65" y="-750.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="365.55" y="-729.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">14 files · 1962 lines</text>
<text xml:space="preserve" text-anchor="start" x="313.6" y="-708.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_engine -->
<g id="node5" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1198.02,-228 877.98,-228 877.98,-48 1198.02,-48 1198.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="1007.97" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="987.78" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 766 lines</text>
</g>
<!-- c_src -->
<g id="node6" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1628.02,-228 1307.98,-228 1307.98,-48 1628.02,-48 1628.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="1454.67" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="1410.55" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1249 lines</text>
</g>
<!-- m_placement&#45;&gt;m_doc -->
<g id="edge4" class="edge">
<title>m_placement&#45;&gt;m_doc</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M287.99,-348.4C287.99,-348.4 287.99,-238.45 287.99,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="290.62,-238.45 287.99,-230.95 285.37,-238.45 290.62,-238.45"/>
</g>
<!-- m_placement&#45;&gt;c_engine -->
<g id="edge3" class="edge">
<title>m_placement&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M328.01,-348.15C328.01,-316.2 328.01,-288 328.01,-288 328.01,-288 1038,-288 1038,-288 1038,-288 1038,-238.38 1038,-238.38"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1040.63,-238.38 1038,-230.88 1035.38,-238.38 1040.63,-238.38"/>
</g>
<!-- m_scene_tool&#45;&gt;m_doc -->
<g id="edge5" class="edge">
<title>m_scene_tool&#45;&gt;m_doc</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M523,-348.4C523,-348.4 523,-238.45 523,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="525.63,-238.45 523,-230.95 520.38,-238.45 525.63,-238.45"/>
</g>
<!-- m_scene_tool&#45;&gt;c_src -->
<g id="edge6" class="edge">
<title>m_scene_tool&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M797.54,-438C1038.64,-438 1468,-438 1468,-438 1468,-438 1468,-238.13 1468,-238.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1470.63,-238.13 1468,-230.63 1465.38,-238.13 1470.63,-238.13"/>
</g>
<!-- c_window&#45;&gt;m_placement -->
<g id="edge1" class="edge">
<title>c_window&#45;&gt;m_placement</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M315.5,-648.4C315.5,-648.4 315.5,-538.45 315.5,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="318.13,-538.45 315.5,-530.95 312.88,-538.45 318.13,-538.45"/>
</g>
<!-- c_window&#45;&gt;m_scene_tool -->
<g id="edge2" class="edge">
<title>c_window&#45;&gt;m_scene_tool</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M530.5,-648.4C530.5,-648.4 530.5,-538.45 530.5,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="533.13,-538.45 530.5,-530.95 527.88,-538.45 533.13,-538.45"/>
</g>
</g>
</svg>
`;case`c_engine`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="1688pt" height="558pt"
 viewBox="0.00 0.00 1688.00 558.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 543.05)">
<g id="clust1" class="cluster">
<title>cluster_c_engine</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-8 8,-289.2 1268,-289.2 1268,-8 8,-8"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-276.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">ENGINE</text>
</g>
<!-- m_frames -->
<g id="node1" class="node">
<title>m_frames</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="798.02,-228 477.98,-228 477.98,-48 798.02,-48 798.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="607.44" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">frames</text>
<text xml:space="preserve" text-anchor="start" x="569.72" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">frames.cpp, frames.hpp</text>
<text xml:space="preserve" text-anchor="start" x="532.37" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Gfx&#39;s frame from a provider, dra</text>
</g>
<!-- m_autana -->
<g id="node2" class="node">
<title>m_autana</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-228 47.98,-228 47.98,-48 368.02,-48 368.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="177.41" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">autana</text>
<text xml:space="preserve" text-anchor="start" x="175.47" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">autana.hpp</text>
<text xml:space="preserve" text-anchor="start" x="95.42" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The one place Loupe includes aut</text>
</g>
<!-- m_pose -->
<g id="node3" class="node">
<title>m_pose</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1228.02,-228 907.98,-228 907.98,-48 1228.02,-48 1228.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="1046.32" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">pose</text>
<text xml:space="preserve" text-anchor="start" x="1041.25" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">pose.hpp</text>
<text xml:space="preserve" text-anchor="start" x="953.58" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Autana&#39;s packages (launcher/pack</text>
</g>
<!-- c_editor -->
<g id="node4" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="798.02,-528 477.98,-528 477.98,-348 798.02,-348 798.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="612.99" y="-439.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="587.78" y="-418.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 547 lines</text>
</g>
<!-- c_path -->
<g id="node5" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1228.02,-528 907.98,-528 907.98,-348 1228.02,-348 1228.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="1048.54" y="-450.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="1017.78" y="-429.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="956.52" y="-408.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_src -->
<g id="node6" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="368.02,-528 47.98,-528 47.98,-348 368.02,-348 368.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="194.67" y="-439.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="150.55" y="-418.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1249 lines</text>
</g>
<!-- c_window -->
<g id="node7" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1658.02,-528 1337.98,-528 1337.98,-348 1658.02,-348 1658.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="1464.65" y="-450.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="1440.55" y="-429.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">14 files · 1962 lines</text>
<text xml:space="preserve" text-anchor="start" x="1388.6" y="-408.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- m_frames&#45;&gt;m_pose -->
<g id="edge7" class="edge">
<title>m_frames&#45;&gt;m_pose</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M797.81,-108C797.81,-108 897.72,-108 897.72,-108"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="897.72,-110.63 905.22,-108 897.72,-105.38 897.72,-110.63"/>
</g>
<!-- m_frames&#45;&gt;c_src -->
<g id="edge6" class="edge">
<title>m_frames&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M478.18,-108C437.31,-108 404.67,-108 404.67,-108 404.67,-108 404.67,-408 404.67,-408 404.67,-408 378.21,-408 378.21,-408"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="378.21,-405.38 370.71,-408 378.21,-410.63 378.21,-405.38"/>
</g>
<!-- c_editor&#45;&gt;m_frames -->
<g id="edge1" class="edge">
<title>c_editor&#45;&gt;m_frames</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M584.66,-348.4C584.66,-348.4 584.66,-238.45 584.66,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="587.29,-238.45 584.66,-230.95 582.04,-238.45 587.29,-238.45"/>
</g>
<!-- c_path&#45;&gt;m_frames -->
<g id="edge2" class="edge">
<title>c_path&#45;&gt;m_frames</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M908.09,-438C876.56,-438 853,-438 853,-438 853,-438 853,-168 853,-168 853,-168 808.44,-168 808.44,-168"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="808.44,-165.38 800.94,-168 808.44,-170.63 808.44,-165.38"/>
</g>
<!-- c_src&#45;&gt;m_frames -->
<g id="edge3" class="edge">
<title>c_src&#45;&gt;m_frames</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M367.82,-468C408.69,-468 441.33,-468 441.33,-468 441.33,-468 441.33,-168 441.33,-168 441.33,-168 467.79,-168 467.79,-168"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="467.79,-170.63 475.29,-168 467.79,-165.38 467.79,-170.63"/>
</g>
<!-- c_src&#45;&gt;m_autana -->
<g id="edge4" class="edge">
<title>c_src&#45;&gt;m_autana</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M208,-348.4C208,-348.4 208,-238.45 208,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="210.63,-238.45 208,-230.95 205.38,-238.45 210.63,-238.45"/>
</g>
<!-- c_window&#45;&gt;m_frames -->
<g id="edge5" class="edge">
<title>c_window&#45;&gt;m_frames</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1498,-348.15C1498,-316.2 1498,-288 1498,-288 1498,-288 691.34,-288 691.34,-288 691.34,-288 691.34,-238.38 691.34,-238.38"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="693.97,-238.38 691.34,-230.88 688.72,-238.38 693.97,-238.38"/>
</g>
</g>
</svg>
`;case`c_path`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="780pt" height="571pt"
 viewBox="0.00 0.00 780.00 571.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 556.25)">
<g id="clust1" class="cluster">
<title>cluster_c_path</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="183.02,-268 183.02,-533.2 567.02,-533.2 567.02,-268 183.02,-268"/>
<text xml:space="preserve" text-anchor="start" x="191.02" y="-520.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">PATH</text>
</g>
<!-- m_path -->
<g id="node1" class="node">
<title>m_path</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="535.04,-480 215,-480 215,-300 535.04,-300 535.04,-480"/>
<text xml:space="preserve" text-anchor="start" x="355.56" y="-402.8" font-family="Arial" font-size="20.00" fill="#f8fafc">path</text>
<text xml:space="preserve" text-anchor="start" x="321.17" y="-381.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">path.cpp, path.hpp</text>
<text xml:space="preserve" text-anchor="start" x="263.54" y="-360.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A scene&#39;s camera path from the c</text>
</g>
<!-- c_src -->
<g id="node2" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-180 0,-180 0,0 320.04,0 320.04,-180"/>
<text xml:space="preserve" text-anchor="start" x="146.69" y="-91.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="102.57" y="-70.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1249 lines</text>
</g>
<!-- c_engine -->
<g id="node3" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="750.04,-180 430,-180 430,0 750.04,0 750.04,-180"/>
<text xml:space="preserve" text-anchor="start" x="559.99" y="-91.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="539.8" y="-70.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 766 lines</text>
</g>
<!-- m_path&#45;&gt;c_src -->
<g id="edge2" class="edge">
<title>m_path&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M250.01,-300.4C250.01,-300.4 250.01,-190.45 250.01,-190.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="252.64,-190.45 250.01,-182.95 247.39,-190.45 252.64,-190.45"/>
</g>
<!-- m_path&#45;&gt;c_engine -->
<g id="edge3" class="edge">
<title>m_path&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M482.52,-300.4C482.52,-300.4 482.52,-190.45 482.52,-190.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="485.15,-190.45 482.52,-182.95 479.9,-190.45 485.15,-190.45"/>
</g>
<!-- c_src&#45;&gt;m_path -->
<g id="edge1" class="edge">
<title>c_src&#45;&gt;m_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M285.03,-179.6C285.03,-179.6 285.03,-289.55 285.03,-289.55"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="282.4,-289.55 285.03,-297.05 287.65,-289.55 282.4,-289.55"/>
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
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1180.04,-1128 860,-1128 860,-948 1180.04,-948 1180.04,-1128"/>
<text xml:space="preserve" text-anchor="start" x="998.9" y="-1050.8" font-family="Arial" font-size="20.00" fill="#f8fafc">pack</text>
<text xml:space="preserve" text-anchor="start" x="964.01" y="-1029.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">pack.cpp, pack.hpp</text>
<text xml:space="preserve" text-anchor="start" x="911.03" y="-1008.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A scene&#39;s asset pack: a .apak as</text>
</g>
<!-- m_tools -->
<g id="node7" class="node">
<title>m_tools</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1180.04,-828 860,-828 860,-648 1180.04,-648 1180.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="998.9" y="-750.8" font-family="Arial" font-size="20.00" fill="#f8fafc">tools</text>
<text xml:space="preserve" text-anchor="start" x="964.01" y="-729.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">tools.cpp, tools.hpp</text>
<text xml:space="preserve" text-anchor="start" x="910.79" y="-708.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The programs a build with the pa</text>
</g>
<!-- m_process -->
<g id="node8" class="node">
<title>m_process</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1180.04,-528 860,-528 860,-348 1180.04,-348 1180.04,-528"/>
<text xml:space="preserve" text-anchor="start" x="985.01" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">process</text>
<text xml:space="preserve" text-anchor="start" x="945.95" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">process.cpp, process.hpp</text>
<text xml:space="preserve" text-anchor="start" x="912.04" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Other processes: the tools loupe</text>
</g>
<!-- m_paths -->
<g id="node9" class="node">
<title>m_paths</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1180.04,-228 860,-228 860,-48 1180.04,-48 1180.04,-228"/>
<text xml:space="preserve" text-anchor="start" x="995.56" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">paths</text>
<text xml:space="preserve" text-anchor="start" x="959.67" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">paths.cpp, paths.hpp</text>
<text xml:space="preserve" text-anchor="start" x="912.05" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">A path as UTF&#45;8 text and back: t</text>
</g>
<!-- c_editor -->
<g id="node10" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="350.04,-1128 30,-1128 30,-948 350.04,-948 350.04,-1128"/>
<text xml:space="preserve" text-anchor="start" x="165.01" y="-1039.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="139.8" y="-1018.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 547 lines</text>
</g>
<!-- c_engine -->
<g id="node11" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2870.04,-828 2550,-828 2550,-648 2870.04,-648 2870.04,-828"/>
<text xml:space="preserve" text-anchor="start" x="2679.99" y="-739.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="2659.8" y="-718.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 766 lines</text>
</g>
<!-- c_update -->
<g id="node12" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2040.04,-1728 1720,-1728 1720,-1548 2040.04,-1548 2040.04,-1728"/>
<text xml:space="preserve" text-anchor="start" x="1849.43" y="-1650.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="1829.8" y="-1629.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 853 lines</text>
<text xml:space="preserve" text-anchor="start" x="1777.05" y="-1608.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_path -->
<g id="node13" class="node">
<title>c_path</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="320.04,-1428 0,-1428 0,-1248 320.04,-1248 320.04,-1428"/>
<text xml:space="preserve" text-anchor="start" x="140.56" y="-1350.8" font-family="Arial" font-size="20.00" fill="#eff6ff">path</text>
<text xml:space="preserve" text-anchor="start" x="109.8" y="-1329.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">2 files · 404 lines</text>
<text xml:space="preserve" text-anchor="start" x="48.54" y="-1308.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">A scene&#39;s camera path from the c</text>
</g>
<!-- c_window -->
<g id="node14" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="1395.04,-1728 1075,-1728 1075,-1548 1395.04,-1548 1395.04,-1728"/>
<text xml:space="preserve" text-anchor="start" x="1201.67" y="-1650.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="1177.57" y="-1629.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">14 files · 1962 lines</text>
<text xml:space="preserve" text-anchor="start" x="1125.62" y="-1608.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- m_main&#45;&gt;c_path -->
<g id="edge6" class="edge">
<title>m_main&#45;&gt;c_path</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M430.21,-1368C430.21,-1368 330.3,-1368 330.3,-1368"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="330.3,-1365.38 322.8,-1368 330.3,-1370.63 330.3,-1365.38"/>
</g>
<!-- m_main&#45;&gt;c_window -->
<g id="edge7" class="edge">
<title>m_main&#45;&gt;c_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M590.02,-1427.6C590.02,-1516 590.02,-1638 590.02,-1638 590.02,-1638 1064.64,-1638 1064.64,-1638"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1064.64,-1640.63 1072.14,-1638 1064.64,-1635.38 1064.64,-1640.63"/>
</g>
<!-- m_options&#45;&gt;m_pack -->
<g id="edge13" class="edge">
<title>m_options&#45;&gt;m_pack</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2310.02,-1248.09C2310.02,-1169.55 2310.02,-1068 2310.02,-1068 2310.02,-1068 1190.34,-1068 1190.34,-1068"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1190.34,-1065.38 1182.84,-1068 1190.34,-1070.63 1190.34,-1065.38"/>
</g>
<!-- m_library&#45;&gt;m_pack -->
<g id="edge14" class="edge">
<title>m_library&#45;&gt;m_pack</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1020.02,-1248.4C1020.02,-1248.4 1020.02,-1138.45 1020.02,-1138.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1022.65,-1138.45 1020.02,-1130.95 1017.4,-1138.45 1022.65,-1138.45"/>
</g>
<!-- m_pack&#45;&gt;m_tools -->
<g id="edge16" class="edge">
<title>m_pack&#45;&gt;m_tools</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1020.02,-948.4C1020.02,-948.4 1020.02,-838.45 1020.02,-838.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1022.65,-838.45 1020.02,-830.95 1017.4,-838.45 1022.65,-838.45"/>
</g>
<!-- m_pack&#45;&gt;c_engine -->
<g id="edge15" class="edge">
<title>m_pack&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1179.93,-1008C1591.76,-1008 2656.68,-1008 2656.68,-1008 2656.68,-1008 2656.68,-838.43 2656.68,-838.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2659.31,-838.43 2656.68,-830.93 2654.06,-838.43 2659.31,-838.43"/>
</g>
<!-- m_tools&#45;&gt;m_process -->
<g id="edge17" class="edge">
<title>m_tools&#45;&gt;m_process</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1020.02,-648.4C1020.02,-648.4 1020.02,-538.45 1020.02,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1022.65,-538.45 1020.02,-530.95 1017.4,-538.45 1022.65,-538.45"/>
</g>
<!-- m_process&#45;&gt;m_paths -->
<g id="edge18" class="edge">
<title>m_process&#45;&gt;m_paths</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1020.02,-348.4C1020.02,-348.4 1020.02,-238.45 1020.02,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1022.65,-238.45 1020.02,-230.95 1017.4,-238.45 1022.65,-238.45"/>
</g>
<!-- c_editor&#45;&gt;m_tools -->
<g id="edge1" class="edge">
<title>c_editor&#45;&gt;m_tools</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M190.02,-948.4C190.02,-860 190.02,-738 190.02,-738 190.02,-738 849.95,-738 849.95,-738"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="849.95,-740.63 857.45,-738 849.95,-735.38 849.95,-740.63"/>
</g>
<!-- c_engine&#45;&gt;m_options -->
<g id="edge2" class="edge">
<title>c_engine&#45;&gt;m_options</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2763.36,-827.83C2763.36,-995.21 2763.36,-1338 2763.36,-1338 2763.36,-1338 2480.38,-1338 2480.38,-1338"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2480.38,-1335.38 2472.88,-1338 2480.38,-1340.63 2480.38,-1335.38"/>
</g>
<!-- c_update&#45;&gt;m_options -->
<g id="edge3" class="edge">
<title>c_update&#45;&gt;m_options</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2039.83,-1638C2162.15,-1638 2310.02,-1638 2310.02,-1638 2310.02,-1638 2310.02,-1438.13 2310.02,-1438.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2312.65,-1438.13 2310.02,-1430.63 2307.4,-1438.13 2312.65,-1438.13"/>
</g>
<!-- c_update&#45;&gt;m_scratch -->
<g id="edge4" class="edge">
<title>c_update&#45;&gt;m_scratch</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1720.21,-1608C1617.26,-1608 1502.54,-1608 1502.54,-1608 1502.54,-1608 1502.54,-1438.43 1502.54,-1438.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1505.17,-1438.43 1502.54,-1430.93 1499.92,-1438.43 1505.17,-1438.43"/>
</g>
<!-- c_update&#45;&gt;m_version -->
<g id="edge5" class="edge">
<title>c_update&#45;&gt;m_version</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1880.02,-1548.4C1880.02,-1548.4 1880.02,-1438.45 1880.02,-1438.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1882.65,-1438.45 1880.02,-1430.95 1877.4,-1438.45 1882.65,-1438.45"/>
</g>
<!-- c_path&#45;&gt;m_scratch -->
<g id="edge8" class="edge">
<title>c_path&#45;&gt;m_scratch</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M160.02,-1427.85C160.02,-1459.8 160.02,-1488 160.02,-1488 160.02,-1488 1325.01,-1488 1325.01,-1488 1325.01,-1488 1325.01,-1438.38 1325.01,-1438.38"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1327.64,-1438.38 1325.01,-1430.88 1322.39,-1438.38 1327.64,-1438.38"/>
</g>
<!-- c_path&#45;&gt;m_pack -->
<g id="edge9" class="edge">
<title>c_path&#45;&gt;m_pack</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M319.76,-1308C359.01,-1308 390.02,-1308 390.02,-1308 390.02,-1308 390.02,-1038 390.02,-1038 390.02,-1038 849.7,-1038 849.7,-1038"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="849.7,-1040.63 857.2,-1038 849.7,-1035.38 849.7,-1040.63"/>
</g>
<!-- c_window&#45;&gt;m_scratch -->
<g id="edge10" class="edge">
<title>c_window&#45;&gt;m_scratch</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1360.03,-1548.4C1360.03,-1548.4 1360.03,-1438.45 1360.03,-1438.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1362.65,-1438.45 1360.03,-1430.95 1357.4,-1438.45 1362.65,-1438.45"/>
</g>
<!-- c_window&#45;&gt;m_version -->
<g id="edge11" class="edge">
<title>c_window&#45;&gt;m_version</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1394.83,-1668C1517.15,-1668 1665.02,-1668 1665.02,-1668 1665.02,-1668 1665.02,-1338 1665.02,-1338 1665.02,-1338 1709.58,-1338 1709.58,-1338"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1709.58,-1340.63 1717.08,-1338 1709.58,-1335.38 1709.58,-1340.63"/>
</g>
<!-- c_window&#45;&gt;m_library -->
<g id="edge12" class="edge">
<title>c_window&#45;&gt;m_library</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1127.52,-1548.4C1127.52,-1548.4 1127.52,-1438.45 1127.52,-1438.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1130.15,-1438.45 1127.52,-1430.95 1124.9,-1438.45 1130.15,-1438.45"/>
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
<svg width="876pt" height="810pt"
 viewBox="0.00 0.00 876.00 810.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 795.05)">
<g id="clust1" class="cluster">
<title>cluster_c_update</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-260 8,-541.2 838,-541.2 838,-260 8,-260"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-528.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">UPDATE</text>
</g>
<!-- m_update -->
<g id="node1" class="node">
<title>m_update</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-480 47.98,-480 47.98,-300 368.02,-300 368.02,-480"/>
<text xml:space="preserve" text-anchor="start" x="177.41" y="-402.8" font-family="Arial" font-size="20.00" fill="#f8fafc">update</text>
<text xml:space="preserve" text-anchor="start" x="139.69" y="-381.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">update.cpp, update.hpp</text>
<text xml:space="preserve" text-anchor="start" x="105.03" y="-360.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">Loupe keeps itself current from</text>
</g>
<!-- m_sha256 -->
<g id="node2" class="node">
<title>m_sha256</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="798.02,-480 477.98,-480 477.98,-300 798.02,-300 798.02,-480"/>
<text xml:space="preserve" text-anchor="start" x="605.19" y="-402.8" font-family="Arial" font-size="20.00" fill="#f8fafc">sha256</text>
<text xml:space="preserve" text-anchor="start" x="566.8" y="-381.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">sha256.cpp, sha256.hpp</text>
<text xml:space="preserve" text-anchor="start" x="528.35" y="-360.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">For checking a download against</text>
</g>
<!-- c_window -->
<g id="node3" class="node">
<title>c_window</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="368.02,-780 47.98,-780 47.98,-600 368.02,-600 368.02,-780"/>
<text xml:space="preserve" text-anchor="start" x="174.65" y="-702.8" font-family="Arial" font-size="20.00" fill="#eff6ff">window</text>
<text xml:space="preserve" text-anchor="start" x="150.55" y="-681.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">14 files · 1962 lines</text>
<text xml:space="preserve" text-anchor="start" x="98.6" y="-660.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">The window: a scene&#39;s frame, a t</text>
</g>
<!-- c_src -->
<g id="node4" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="368.02,-180 47.98,-180 47.98,0 368.02,0 368.02,-180"/>
<text xml:space="preserve" text-anchor="start" x="194.67" y="-91.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="150.55" y="-70.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1249 lines</text>
</g>
<!-- m_update&#45;&gt;m_sha256 -->
<g id="edge2" class="edge">
<title>m_update&#45;&gt;m_sha256</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M367.81,-390C367.81,-390 467.72,-390 467.72,-390"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="467.72,-392.63 475.22,-390 467.72,-387.38 467.72,-392.63"/>
</g>
<!-- m_update&#45;&gt;c_src -->
<g id="edge3" class="edge">
<title>m_update&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M208,-300.4C208,-300.4 208,-190.45 208,-190.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="210.63,-190.45 208,-182.95 205.38,-190.45 210.63,-190.45"/>
</g>
<!-- c_window&#45;&gt;m_update -->
<g id="edge1" class="edge">
<title>c_window&#45;&gt;m_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M208,-600.4C208,-600.4 208,-490.45 208,-490.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="210.63,-490.45 208,-482.95 205.38,-490.45 210.63,-490.45"/>
</g>
</g>
</svg>
`;case`c_window`:return`<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
 "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<!-- Generated by graphviz version 16.1.0 (0)
 -->
<!-- Pages: 1 -->
<svg width="2948pt" height="927pt"
 viewBox="0.00 0.00 2948.00 927.00" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
<g id="graph0" class="graph" transform="scale(1 1) rotate(0) translate(15.05 912.25)">
<g id="clust1" class="cluster">
<title>cluster_c_window</title>
<polygon fill="#194b9e" stroke="#1b3d88" points="8,-8 8,-889.2 1698,-889.2 1698,-8 8,-8"/>
<text xml:space="preserve" text-anchor="start" x="16" y="-876.3" font-family="Arial" font-weight="bold" font-size="11.00" fill="#bfdbfe" fill-opacity="0.701961">WINDOW</text>
</g>
<!-- m_run_window -->
<g id="node1" class="node">
<title>m_run_window</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1228.02,-828 907.98,-828 907.98,-648 1228.02,-648 1228.02,-828"/>
<text xml:space="preserve" text-anchor="start" x="1014.64" y="-750.8" font-family="Arial" font-size="20.00" fill="#f8fafc">run_window</text>
<text xml:space="preserve" text-anchor="start" x="970.08" y="-729.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">run_window.cpp, run_window.hpp</text>
<text xml:space="preserve" text-anchor="start" x="958.6" y="-708.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The window: a scene&#39;s frame, a t</text>
</g>
<!-- m_editor -->
<g id="node2" class="node">
<title>m_editor</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-528 47.98,-528 47.98,-348 368.02,-348 368.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="182.99" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">editor</text>
<text xml:space="preserve" text-anchor="start" x="146.93" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">editor.cpp, editor.hpp</text>
<text xml:space="preserve" text-anchor="start" x="99.84" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The scene editor&#39;s panels over a</text>
</g>
<!-- m_files -->
<g id="node3" class="node">
<title>m_files</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1658.02,-528 1337.98,-528 1337.98,-348 1658.02,-348 1658.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="1480.22" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">files</text>
<text xml:space="preserve" text-anchor="start" x="1446.33" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">files.cpp, files.hpp</text>
<text xml:space="preserve" text-anchor="start" x="1391.1" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The window&#39;s files through SDL:</text>
</g>
<!-- m_frame_texture -->
<g id="node4" class="node">
<title>m_frame_texture</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="798.02,-528 477.98,-528 477.98,-348 798.02,-348 798.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="576.31" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">frame_texture</text>
<text xml:space="preserve" text-anchor="start" x="529.25" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">frame_texture.cpp, frame_texture.hpp</text>
<text xml:space="preserve" text-anchor="start" x="528.59" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The engine&#39;s frame as one textur</text>
</g>
<!-- m_observer -->
<g id="node5" class="node">
<title>m_observer</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="1228.02,-528 907.98,-528 907.98,-348 1228.02,-348 1228.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="1029.09" y="-450.8" font-family="Arial" font-size="20.00" fill="#f8fafc">observer</text>
<text xml:space="preserve" text-anchor="start" x="988.87" y="-429.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">observer.cpp, observer.hpp</text>
<text xml:space="preserve" text-anchor="start" x="957.35" y="-408.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The observer: the person&#39;s own c</text>
</g>
<!-- m_panels -->
<g id="node6" class="node">
<title>m_panels</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="368.02,-228 47.98,-228 47.98,-48 368.02,-48 368.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="178.53" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">panels</text>
<text xml:space="preserve" text-anchor="start" x="141.14" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">panels.cpp, panels.hpp</text>
<text xml:space="preserve" text-anchor="start" x="136.53" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">The window&#39;s panels.</text>
</g>
<!-- m_gpu -->
<g id="node7" class="node">
<title>m_gpu</title>
<polygon fill="#64748b" stroke="#475569" stroke-width="0" points="798.02,-228 477.98,-228 477.98,-48 798.02,-48 798.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="621.32" y="-150.8" font-family="Arial" font-size="20.00" fill="#f8fafc">gpu</text>
<text xml:space="preserve" text-anchor="start" x="587.76" y="-129.8" font-family="Arial" font-size="13.00" fill="#cbd5e1">gpu.cpp, gpu.hpp</text>
<text xml:space="preserve" text-anchor="start" x="524.44" y="-108.2" font-family="Arial" font-size="15.00" fill="#cbd5e1">After imgui&#39;s examples/example_s</text>
</g>
<!-- c_src -->
<g id="node8" class="node">
<title>c_src</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2918.02,-228 2597.98,-228 2597.98,-48 2918.02,-48 2918.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="2744.67" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">src</text>
<text xml:space="preserve" text-anchor="start" x="2700.55" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">17 files · 1249 lines</text>
</g>
<!-- c_update -->
<g id="node9" class="node">
<title>c_update</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2088.02,-528 1767.98,-528 1767.98,-348 2088.02,-348 2088.02,-528"/>
<text xml:space="preserve" text-anchor="start" x="1897.41" y="-450.8" font-family="Arial" font-size="20.00" fill="#eff6ff">update</text>
<text xml:space="preserve" text-anchor="start" x="1877.78" y="-429.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 853 lines</text>
<text xml:space="preserve" text-anchor="start" x="1825.03" y="-408.2" font-family="Arial" font-size="15.00" fill="#bfdbfe">Loupe keeps itself current from</text>
</g>
<!-- c_editor -->
<g id="node10" class="node">
<title>c_editor</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2058.02,-228 1737.98,-228 1737.98,-48 2058.02,-48 2058.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="1872.99" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">editor</text>
<text xml:space="preserve" text-anchor="start" x="1847.78" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">6 files · 547 lines</text>
</g>
<!-- c_engine -->
<g id="node11" class="node">
<title>c_engine</title>
<polygon fill="#3b82f6" stroke="#2563eb" stroke-width="0" points="2488.02,-228 2167.98,-228 2167.98,-48 2488.02,-48 2488.02,-228"/>
<text xml:space="preserve" text-anchor="start" x="2297.97" y="-139.8" font-family="Arial" font-size="20.00" fill="#eff6ff">engine</text>
<text xml:space="preserve" text-anchor="start" x="2277.78" y="-118.8" font-family="Arial" font-size="13.00" fill="#bfdbfe">4 files · 766 lines</text>
</g>
<!-- m_run_window&#45;&gt;m_editor -->
<g id="edge2" class="edge">
<title>m_run_window&#45;&gt;m_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M908,-768C659.25,-768 208,-768 208,-768 208,-768 208,-538.27 208,-538.27"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="210.63,-538.27 208,-530.77 205.38,-538.27 210.63,-538.27"/>
</g>
<!-- m_run_window&#45;&gt;m_files -->
<g id="edge3" class="edge">
<title>m_run_window&#45;&gt;m_files</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1227.81,-693C1350.13,-693 1498,-693 1498,-693 1498,-693 1498,-538.43 1498,-538.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1500.63,-538.43 1498,-530.93 1495.38,-538.43 1500.63,-538.43"/>
</g>
<!-- m_run_window&#45;&gt;m_frame_texture -->
<g id="edge4" class="edge">
<title>m_run_window&#45;&gt;m_frame_texture</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M908.19,-708C785.87,-708 638,-708 638,-708 638,-708 638,-538.43 638,-538.43"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="640.63,-538.43 638,-530.93 635.38,-538.43 640.63,-538.43"/>
</g>
<!-- m_run_window&#45;&gt;m_observer -->
<g id="edge5" class="edge">
<title>m_run_window&#45;&gt;m_observer</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1068,-648.4C1068,-648.4 1068,-538.45 1068,-538.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1070.63,-538.45 1068,-530.95 1065.38,-538.45 1070.63,-538.45"/>
</g>
<!-- m_run_window&#45;&gt;c_update -->
<g id="edge6" class="edge">
<title>m_run_window&#45;&gt;c_update</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1228,-738C1476.75,-738 1928,-738 1928,-738 1928,-738 1928,-538.13 1928,-538.13"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1930.63,-538.13 1928,-530.63 1925.38,-538.13 1930.63,-538.13"/>
</g>
<!-- m_editor&#45;&gt;m_panels -->
<g id="edge7" class="edge">
<title>m_editor&#45;&gt;m_panels</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M154.66,-348.4C154.66,-348.4 154.66,-238.45 154.66,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="157.29,-238.45 154.66,-230.95 152.04,-238.45 157.29,-238.45"/>
</g>
<!-- m_editor&#45;&gt;c_editor -->
<g id="edge8" class="edge">
<title>m_editor&#45;&gt;c_editor</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M261.34,-348.27C261.34,-300.71 261.34,-252 261.34,-252 261.34,-252 1752.98,-252 1752.98,-252 1752.98,-252 1752.98,-238.35 1752.98,-238.35"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1755.61,-238.35 1752.98,-230.85 1750.36,-238.35 1755.61,-238.35"/>
</g>
<!-- m_files&#45;&gt;c_src -->
<g id="edge9" class="edge">
<title>m_files&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1498,-348.18C1498,-334.05 1498,-324 1498,-324 1498,-324 2704.66,-324 2704.66,-324 2704.66,-324 2704.66,-238.26 2704.66,-238.26"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2707.29,-238.26 2704.66,-230.76 2702.04,-238.26 2707.29,-238.26"/>
</g>
<!-- m_frame_texture&#45;&gt;m_gpu -->
<g id="edge10" class="edge">
<title>m_frame_texture&#45;&gt;m_gpu</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M584.66,-348.4C584.66,-348.4 584.66,-238.45 584.66,-238.45"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="587.29,-238.45 584.66,-230.95 582.04,-238.45 587.29,-238.45"/>
</g>
<!-- m_frame_texture&#45;&gt;c_engine -->
<g id="edge11" class="edge">
<title>m_frame_texture&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M691.34,-348.47C691.34,-311 691.34,-276 691.34,-276 691.34,-276 2274.66,-276 2274.66,-276 2274.66,-276 2274.66,-238.26 2274.66,-238.26"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2277.29,-238.26 2274.66,-230.76 2272.04,-238.26 2277.29,-238.26"/>
</g>
<!-- m_observer&#45;&gt;c_engine -->
<g id="edge12" class="edge">
<title>m_observer&#45;&gt;c_engine</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M1068,-348.27C1068,-321.91 1068,-300 1068,-300 1068,-300 2381.34,-300 2381.34,-300 2381.34,-300 2381.34,-238.06 2381.34,-238.06"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2383.97,-238.06 2381.34,-230.56 2378.72,-238.06 2383.97,-238.06"/>
</g>
<!-- m_panels&#45;&gt;c_src -->
<g id="edge13" class="edge">
<title>m_panels&#45;&gt;c_src</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M208,-48.32C208,-37.4 208,-30 208,-30 208,-30 2758,-30 2758,-30 2758,-30 2758,-37.79 2758,-37.79"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="2755.38,-37.79 2758,-45.29 2760.63,-37.79 2755.38,-37.79"/>
</g>
<!-- c_src&#45;&gt;m_run_window -->
<g id="edge1" class="edge">
<title>c_src&#45;&gt;m_run_window</title>
<path fill="none" stroke="#64748b" stroke-width="2" d="M2811.34,-227.68C2811.34,-404.91 2811.34,-783 2811.34,-783 2811.34,-783 1238.25,-783 1238.25,-783"/>
<polygon fill="#64748b" stroke="#64748b" stroke-width="2" points="1238.25,-780.38 1230.75,-783 1238.25,-785.63 1238.25,-780.38"/>
</g>
</g>
</svg>
`;default:throw Error(`Unknown viewId: `+e)}};export{e as dotSource,t as svgSource};