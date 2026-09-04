package mkvlib

import (
	"path"
	"regexp"
	"testing"
)

func TestPgsOutputPathUsesSupExtension(t *testing.T) {
	got := pgsOutputPath("output", "subtitle")
	want := path.Join("output", "subtitle.sup")
	if got != want {
		t.Fatalf("pgsOutputPath() = %q, want %q", got, want)
	}
}

func TestPgsSubtitlePatternAcceptsSupAndLegacyPgs(t *testing.T) {
	tests := []struct {
		name string
		path string
		want bool
	}{
		{name: "new sup output", path: "subtitle.sup", want: true},
		{name: "legacy pgs output", path: "subtitle.pgs", want: true},
		{name: "ASS source", path: "subtitle.ass", want: false},
		{name: "suffix must be final", path: "subtitle.sup.bak", want: false},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got := regexp.MustCompile(pgsSubtitlePattern).MatchString(tt.path)
			if got != tt.want {
				t.Fatalf("pattern match for %q = %t, want %t", tt.path, got, tt.want)
			}
		})
	}
}

func TestMuxSubtitlePatternAcceptsSupportedSubtitleExtensions(t *testing.T) {
	tests := []struct {
		name string
		path string
		want bool
	}{
		{name: "VobSub", path: "subtitle.sub", want: true},
		{name: "legacy PGS", path: "subtitle.pgs", want: true},
		{name: "new SUP", path: "subtitle.sup", want: true},
		{name: "ASS handled separately", path: "subtitle.ass", want: false},
		{name: "suffix must be final", path: "subtitle.sup.bak", want: false},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got := regexp.MustCompile(muxSubtitlePattern).MatchString(tt.path)
			if got != tt.want {
				t.Fatalf("pattern match for %q = %t, want %t", tt.path, got, tt.want)
			}
		})
	}
}
