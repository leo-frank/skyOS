# ------------------------------------------------------------------------------
# Rules
# ------------------------------------------------------------------------------
run:
	@make -C user
	@make -C kernel run

debug:
	@make -C kernel debug

user:
	@make -C user

clean:
	@make -C kernel clean
	@make -C user clean